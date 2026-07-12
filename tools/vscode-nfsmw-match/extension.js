const vscode = require("vscode");
const cp = require("child_process");
const crypto = require("crypto");
const fs = require("fs");
const path = require("path");
const os = require("os");
const util = require("util");

const execFile = util.promisify(cp.execFile);
const ANNOTATION_CACHE_VERSION = 6;
const FUNCTION_KINDS = new Set([
  vscode.SymbolKind.Function,
  vscode.SymbolKind.Method,
  vscode.SymbolKind.Constructor,
]);
const CONTAINER_KINDS = new Set([
  vscode.SymbolKind.Namespace,
  vscode.SymbolKind.Class,
  vscode.SymbolKind.Struct,
  vscode.SymbolKind.Interface,
]);
const symbolContainers = new WeakMap();

function normalizedPath(value) {
  return value.replace(/\\/g, "/").replace(/^\.\//, "").toLowerCase();
}

function functionLeaf(name) {
  const parametersAt = name.lastIndexOf("(");
  const bare = name.slice(0, parametersAt === -1 ? undefined : parametersAt);
  return bare.trim().split("::").pop().replace(/^~/, "~");
}

function flattenSymbols(symbols, output = [], containers = []) {
  for (const symbol of symbols || []) {
    if (FUNCTION_KINDS.has(symbol.kind)) {
      symbolContainers.set(symbol, containers.join("::"));
      output.push(symbol);
    }
    const childContainers = CONTAINER_KINDS.has(symbol.kind)
      ? [...containers, functionLeaf(symbol.name || "")]
      : containers;
    if (symbol.children) flattenSymbols(symbol.children, output, childContainers);
  }
  return output;
}

function symbolName(symbol) {
  const container = symbol.containerName || symbolContainers.get(symbol) || "";
  return `${container}::${symbol.name || ""} ${symbol.detail || ""}`;
}

function symbolRange(symbol) {
  return symbol.selectionRange || symbol.range || symbol.location?.range;
}

function symbolSourcePrefix(document, symbol) {
  const range = symbolRange(symbol);
  if (!range) return "";
  const endLine = Math.min(range.end.line, range.start.line + 8);
  return document.getText(
    new vscode.Range(range.start, new vscode.Position(endLine, document.lineAt(endLine).text.length)),
  );
}

function scoreSymbol(functionName, symbol, document) {
  const parameterAt = functionName.lastIndexOf("(");
  const wantedName = functionName.slice(0, parameterAt === -1 ? undefined : parameterAt);
  const wanted = wantedName.replace(/\s+/g, "").toLowerCase();
  const candidate = `${symbolName(symbol)} ${symbolSourcePrefix(document, symbol)}`
    .replace(/\s+/g, "")
    .toLowerCase();
  const leaf = functionLeaf(functionName).toLowerCase();
  const candidateLeaf = functionLeaf(symbol.name || "").toLowerCase();
  if (candidate.includes(wanted)) return 100;
  if (candidateLeaf !== leaf) return -1;
  const ownerAt = wantedName.lastIndexOf("::");
  if (ownerAt !== -1) {
    const owner = wantedName.slice(0, ownerAt).replace(/\s+/g, "").toLowerCase();
    return candidate.includes(`${owner}::`) ? 20 : -1;
  }
  return 10;
}

async function locateFunctions(document, functionNames) {
  const raw = await vscode.commands.executeCommand(
    "vscode.executeDocumentSymbolProvider",
    document.uri,
  );
  const symbols = flattenSymbols(raw || []);
  const used = new Set();
  const locations = new Map();

  for (const name of functionNames) {
    let best;
    let bestScore = -1;
    for (let index = 0; index < symbols.length; index += 1) {
      if (used.has(index)) continue;
      const score = scoreSymbol(name, symbols[index], document);
      if (score > bestScore) {
        best = index;
        bestScore = score;
      }
    }
    if (best !== undefined && bestScore >= 0) {
      used.add(best);
      locations.set(
        name,
        symbolRange(symbols[best]),
      );
    }
  }
  return locations;
}

async function locateFunctionOccurrences(document, functionNames) {
  const raw = await vscode.commands.executeCommand(
    "vscode.executeDocumentSymbolProvider",
    document.uri,
  );
  const symbols = flattenSymbols(raw || []);
  const locations = new Map();

  for (const name of functionNames) {
    const scored = symbols
      .map((symbol) => ({ symbol, score: scoreSymbol(name, symbol, document) }))
      .filter((candidate) => candidate.score >= 0);
    const bestScore = scored.reduce((best, candidate) => Math.max(best, candidate.score), -1);
    const seen = new Set();
    const ranges = scored
      .filter((candidate) => candidate.score === bestScore)
      .map((candidate) => symbolRange(candidate.symbol))
      .filter((range) => {
        if (!range) return false;
        const key = `${range.start.line}:${range.start.character}:${range.end.line}:${range.end.character}`;
        if (seen.has(key)) return false;
        seen.add(key);
        return true;
      });
    if (ranges.length) locations.set(name, ranges);
  }
  return locations;
}

function collectTypeSymbols(symbols, output = [], containers = []) {
  for (const symbol of symbols || []) {
    const isType = symbol.kind === vscode.SymbolKind.Class || symbol.kind === vscode.SymbolKind.Struct;
    const isNamespace = symbol.kind === vscode.SymbolKind.Namespace;
    const name = (symbol.name || "").trim();
    const qualifiedName = [...containers, name].filter(Boolean).join("::");
    if (isType && symbolRange(symbol)) {
      output.push({ name: qualifiedName, range: symbolRange(symbol) });
    }
    const childContainers = isType || isNamespace ? [...containers, name] : containers;
    if (symbol.children) collectTypeSymbols(symbol.children, output, childContainers);
  }
  return output;
}

async function locateTypes(document) {
  const raw = await vscode.commands.executeCommand(
    "vscode.executeDocumentSymbolProvider",
    document.uri,
  );
  return new Map(collectTypeSymbols(raw || []).map((item) => [item.name, item.range]));
}

function repoForDocument(document) {
  const folder = vscode.workspace.getWorkspaceFolder(document.uri);
  if (!folder) return undefined;
  let current = path.dirname(document.uri.fsPath);
  const stop = path.resolve(folder.uri.fsPath);
  while (current.startsWith(stop)) {
    if (fs.existsSync(path.join(current, "objdiff.json"))) return current;
    if (current === stop) break;
    current = path.dirname(current);
  }
  return fs.existsSync(path.join(stop, "objdiff.json")) ? stop : undefined;
}

function resolveUnityInclude(repo, unitySource, includePath) {
  const candidates = [
    path.resolve(path.dirname(unitySource), includePath),
    path.resolve(repo, "src", includePath),
    path.resolve(repo, includePath),
  ];
  return candidates.find((candidate) => fs.existsSync(candidate));
}

function unityIncludesSource(repo, unitySource, wantedSource, visited = new Set()) {
  const canonicalUnity = path.resolve(unitySource).toLowerCase();
  if (visited.has(canonicalUnity)) return false;
  visited.add(canonicalUnity);

  let text;
  try {
    text = fs.readFileSync(unitySource, "utf8");
  } catch (_) {
    return false;
  }
  const includePattern = /^\s*#\s*include\s*"([^"]+)"/gm;
  for (const match of text.matchAll(includePattern)) {
    const included = resolveUnityInclude(repo, unitySource, match[1]);
    if (!included) continue;
    if (path.resolve(included).toLowerCase() === path.resolve(wantedSource).toLowerCase()) {
      return true;
    }
    // Unity lists occasionally include another source list. Follow source files,
    // but do not crawl the much larger header include graph.
    if (/\.(?:c|cc|cp|cpp|cxx|c\+\+)$/i.test(included)) {
      if (unityIncludesSource(repo, included, wantedSource, visited)) return true;
    }
  }
  return false;
}

function unitsForDocument(repo, document) {
  const config = JSON.parse(fs.readFileSync(path.join(repo, "objdiff.json"), "utf8"));
  const relative = normalizedPath(path.relative(repo, document.uri.fsPath));
  const directMatches = (config.units || []).filter(
    (unit) => normalizedPath(unit.metadata?.source_path || "") === relative,
  );
  if (directMatches.length) return directMatches;

  const units = config.units || [];
  const sourceListUnits = units.filter((unit) =>
    normalizedPath(unit.metadata?.source_path || "").includes("/sourcelists/"),
  );
  const includedBy = (candidates) => candidates.filter((unit) => {
    const unitySource = path.resolve(repo, unit.metadata.source_path);
    return unityIncludesSource(repo, unitySource, document.uri.fsPath);
  });
  let matches = includedBy(sourceListUnits);
  if (matches.length === 0) {
    const sourceListNames = new Set(sourceListUnits.map((unit) => unit.name));
    matches = includedBy(
      units.filter(
        (unit) => unit.metadata?.source_path && !sourceListNames.has(unit.name),
      ),
    );
  }
  if (!matches.length) {
    throw new Error(`No objdiff translation unit directly or transitively includes '${relative}'.`);
  }
  return matches;
}

async function runJson(python, script, args, cwd, signal) {
  const { stdout, stderr } = await execFile(python, [script, ...args], {
    cwd,
    encoding: "utf8",
    maxBuffer: 128 * 1024 * 1024,
    windowsHide: true,
    signal,
  });
  try {
    return JSON.parse(stdout);
  } catch (error) {
    throw new Error(`${path.basename(script)} returned invalid JSON: ${error.message}\n${stderr}`);
  }
}

class AnnotationCache {
  constructor(context) {
    this.directory = context.globalStorageUri.fsPath;
  }

  limit() {
    const configured = vscode.workspace.getConfiguration("nfsmwMatch").get("cacheMaxMB", 500);
    return Math.min(500, Math.max(1, configured)) * 1024 * 1024;
  }

  key(repo, source) {
    return crypto.createHash("sha256").update(`${repo}\0${source}`).digest("hex");
  }

  async read(repo, source) {
    const filename = path.join(this.directory, `${this.key(repo, source)}.json`);
    try {
      const value = JSON.parse(await fs.promises.readFile(filename, "utf8"));
      const stat = await fs.promises.stat(source);
      if (
        value.cacheVersion !== ANNOTATION_CACHE_VERSION ||
        value.sourceMtimeMs !== stat.mtimeMs
      ) return undefined;
      await fs.promises.utimes(filename, new Date(), new Date());
      return value;
    } catch (_) {
      return undefined;
    }
  }

  async write(repo, source, value) {
    await fs.promises.mkdir(this.directory, { recursive: true });
    const stat = await fs.promises.stat(source);
    const filename = path.join(this.directory, `${this.key(repo, source)}.json`);
    const temporary = `${filename}.${process.pid}.tmp`;
    const serialized = JSON.stringify({
      ...value,
      cacheVersion: ANNOTATION_CACHE_VERSION,
      sourceMtimeMs: stat.mtimeMs,
    });
    const size = Buffer.byteLength(serialized, "utf8");
    if (size > this.limit()) return;
    await this.prune(size);
    await fs.promises.writeFile(temporary, serialized, "utf8");
    await fs.promises.rename(temporary, filename);
    await this.prune();
  }

  async prune(reservedBytes = 0) {
    await fs.promises.mkdir(this.directory, { recursive: true });
    const entries = [];
    for (const name of await fs.promises.readdir(this.directory)) {
      const filename = path.join(this.directory, name);
      try {
        const stat = await fs.promises.stat(filename);
        if (stat.isFile()) {
          entries.push({ filename, size: stat.size, time: stat.atimeMs || stat.mtimeMs });
        }
      } catch (_) {}
    }
    entries.sort((a, b) => a.time - b.time);
    let total = entries.reduce((sum, entry) => sum + entry.size, 0);
    for (const entry of entries) {
      if (total + reservedBytes <= this.limit()) break;
      await fs.promises.unlink(entry.filename).catch(() => {});
      total -= entry.size;
    }
  }
}

class MatchCodeLensProvider {
  constructor() {
    this.values = new Map();
    this.changed = new vscode.EventEmitter();
    this.onDidChangeCodeLenses = this.changed.event;
  }

  set(uri, annotations) {
    this.values.set(uri.toString(), annotations);
    this.changed.fire();
  }

  clear(uri) {
    if (this.values.delete(uri.toString())) this.changed.fire();
  }

  provideCodeLenses(document) {
    const annotations = this.values.get(document.uri.toString()) || [];
    const lenses = [];
    for (const annotation of annotations) {
      if (annotation.kind === "type") {
        const match = annotation.typePercent == null ? "not found" : `${annotation.typePercent.toFixed(1)}%`;
        lenses.push(new vscode.CodeLens(annotation.range, {
          title: `$(symbol-struct) PS2 type ${match}`,
          command: annotation.comparisonDiff ? "nfsmwMatch.showDwarfDiff" : "",
          arguments: annotation.comparisonDiff ? [annotation] : [],
          tooltip: annotation.error || `${annotation.type}\nPS2 struct definition ${match}`,
        }));
        continue;
      }
      const objdiff = annotation.objdiffPercent == null ? "n/a" : `${annotation.objdiffPercent.toFixed(1)}%`;
      const hasObjdiff = annotation.objdiffPercent != null;
      if (annotation.platform === "ps2") {
        if (!hasObjdiff) continue;
        lenses.push(new vscode.CodeLens(annotation.range, {
          title: `$(pulse) objdiff ${objdiff}`,
          command: "",
          tooltip: `${annotation.function}\nobjdiff ${objdiff}; DWARF unavailable on PS2`,
        }));
        continue;
      }
      const dwarf = annotation.dwarfPercent == null ? "not found" : `${annotation.dwarfPercent.toFixed(1)}%`;
      const tooltip = annotation.error || `${annotation.function}\n${hasObjdiff ? `objdiff ${objdiff}; ` : ""}DWARF ${dwarf}`;
      lenses.push(new vscode.CodeLens(annotation.range, {
        title: hasObjdiff ? `$(pulse) objdiff ${objdiff}  ·  DWARF ${dwarf}` : `$(pulse) DWARF ${dwarf}`,
        command: annotation.dwarfDiff ? "nfsmwMatch.showDwarfDiff" : "",
        arguments: annotation.dwarfDiff ? [annotation] : [],
        tooltip,
      }));
    }
    return lenses;
  }
}

class DwarfDiffProvider {
  constructor() {
    this.content = new Map();
    this.changed = new vscode.EventEmitter();
    this.onDidChange = this.changed.event;
  }
  provideTextDocumentContent(uri) {
    return this.content.get(uri.toString()) || "Comparison diff is no longer available.";
  }
  uriFor(annotation) {
    const itemName = annotation.function || annotation.type;
    const id = crypto.createHash("sha1").update(`${annotation.unit}\0${itemName}`).digest("hex");
    const uri = vscode.Uri.parse(`nfsmw-dwarf-diff:${id}/${encodeURIComponent(functionLeaf(itemName))}.diff`);
    const content = annotation.comparisonDiff || annotation.dwarfDiff;
    if (this.content.get(uri.toString()) !== content) {
      this.content.set(uri.toString(), content);
      this.changed.fire(uri);
    }
    return uri;
  }

  dispose() {
    this.changed.dispose();
  }
}

async function activate(context) {
  const lenses = new MatchCodeLensProvider();
  const diffs = new DwarfDiffProvider();
  const cache = new AnnotationCache(context);
  const diagnostics = vscode.languages.createDiagnosticCollection("nfsmw-match");
  const runningRefreshes = new Map();
  const lastResults = new Map();
  let lastDiffViewColumn;
  const selector = [{ language: "c", scheme: "file" }, { language: "cpp", scheme: "file" }];
  context.subscriptions.push(
    vscode.languages.registerCodeLensProvider(selector, lenses),
    vscode.workspace.registerTextDocumentContentProvider("nfsmw-dwarf-diff", diffs),
    diffs,
    diagnostics,
  );

  async function applyResult(document, result) {
    lastResults.set(document.uri.toString(), result);
    const functionItems = result.annotations.filter((item) => item.kind !== "type");
    const typeItems = result.annotations.filter((item) => item.kind === "type");
    const locations = await locateFunctionOccurrences(document, functionItems.map((item) => item.function));
    const typeLocations = typeItems.length ? await locateTypes(document) : new Map();
    const annotations = [
      ...functionItems
        .flatMap((item) => (locations.get(item.function) || [])
          .map((range) => ({ ...item, range }))),
      ...typeItems
        .filter((item) => typeLocations.get(item.type))
        .map((item) => ({ ...item, range: typeLocations.get(item.type) })),
    ];
    lenses.set(document.uri, annotations);
    const errors = [];
    for (const annotation of annotations) {
      const mismatches = [
        ["objdiff", annotation.objdiffPercent],
        [annotation.kind === "type" ? "PS2 type" : "DWARF", annotation.kind === "type" ? annotation.typePercent : annotation.dwarfPercent],
      ].filter(([, value]) => value != null && Math.abs(value - 100) > 0.000001);
      if (!mismatches.length) continue;
      const line = annotation.range.start.line;
      const lineRange = document.lineAt(line).range;
      errors.push(new vscode.Diagnostic(
        lineRange,
        `${mismatches.map(([name, value]) => `${name} ${value.toFixed(1)}%`).join("; ")} is not a full match`,
        vscode.DiagnosticSeverity.Warning,
      ));
    }
    diagnostics.set(document.uri, errors);
    return annotations.length;
  }

  async function selectStdumpPath(forcePrompt = false) {
    const configuration = vscode.workspace.getConfiguration("nfsmwMatch");
    const configured = configuration.get("stdumpPath", "");
    if (!forcePrompt && configured && fs.existsSync(configured)) return configured;
    const selected = await vscode.window.showOpenDialog({
      title: "Select ccc stdump executable for PS2 type comparisons",
      canSelectFiles: true,
      canSelectFolders: false,
      canSelectMany: false,
      filters: process.platform === "win32" ? { Executables: ["exe"] } : undefined,
    });
    if (!selected?.length) throw new Error("A stdump executable is required for PS2 type comparisons.");
    const stdumpPath = selected[0].fsPath;
    await configuration.update("stdumpPath", stdumpPath, vscode.ConfigurationTarget.Global);
    return stdumpPath;
  }

  async function refreshOnce(document, force, signal, cancel) {
    const repo = repoForDocument(document);
    if (!repo) return;
    const candidateUnits = unitsForDocument(repo, document);
    const candidateKey = candidateUnits
      .map((candidate) => `${candidate.name}:${candidate.scratch?.platform || ""}`)
      .sort()
      .join("|");
    const objdiffEnabled = vscode.workspace.getConfiguration("nfsmwMatch").get("objdiffEnabled", true);
    const cacheKey = `${candidateKey}|objdiff:${objdiffEnabled}`;
    if (!force) {
      const cached = await cache.read(repo, document.uri.fsPath);
      if (cached && cached.candidateKey === cacheKey) {
        await applyResult(document, cached);
        return;
      }
    }

    const python = vscode.workspace.getConfiguration("nfsmwMatch").get("pythonPath", "python");
    const progress = force ? vscode.window.withProgress.bind(vscode.window) : async (_, task) => task();
    await progress(
      { location: vscode.ProgressLocation.Notification, title: `Refreshing match annotations for ${path.basename(document.fileName)}`, cancellable: true },
      async (_, token) => {
        token?.onCancellationRequested(cancel);
        const inspectedUnits = [];
        for (const candidate of candidateUnits) {
          if (signal.aborted) throw new vscode.CancellationError();
          const rows = objdiffEnabled ? await runJson(
            python, path.join(repo, "tools", "decomp-diff.py"),
            ["-u", candidate.name, "-t", "function", "--json"], repo, signal,
          ) : [];
          const sourceLocations = await locateFunctions(document, rows.map((row) => row.name));
          inspectedUnits.push({
            config: candidate,
            sourceRows: rows.filter((row) => sourceLocations.get(row.name)),
          });
        }
        inspectedUnits.sort((left, right) => right.sourceRows.length - left.sourceRows.length);
        const selectedUnit = inspectedUnits[0];
        let unit = selectedUnit.config.name;
        const platform = selectedUnit.config.scratch?.platform;
        const sourceRows = selectedUnit.sourceRows;
        let functionNames = sourceRows.map((row) => row.name);
        if (!objdiffEnabled && platform !== "ps2") {
          const rawSymbols = await vscode.commands.executeCommand("vscode.executeDocumentSymbolProvider", document.uri);
          functionNames = [...new Set(flattenSymbols(rawSymbols || []).map((symbol) => {
            const container = symbol.containerName || symbolContainers.get(symbol) || "";
            return [container, symbol.name].filter(Boolean).join("::");
          }))];
        }
        if (functionNames.length === 0 && platform !== "ps2") {
          throw new Error(objdiffEnabled
            ? "No objdiff functions could be matched to document symbols in this file."
            : "No function definitions could be matched to document symbols in this file.");
        }

        if (platform === "ps2") {
          const annotations = sourceRows.map((row) => ({
            kind: "function",
            platform,
            unit,
            function: row.name,
            objdiffPercent: row.match_percent,
          }));
          const typeLocations = await locateTypes(document);
          const typeNames = [...typeLocations.keys()];
          if (typeNames.length) {
            const stdumpPath = await selectStdumpPath();
            const requestFile = path.join(os.tmpdir(), `nfsmw-types-${process.pid}-${Date.now()}.json`);
            await fs.promises.writeFile(requestFile, JSON.stringify(typeNames), "utf8");
            const bestReports = new Map();
            try {
              for (const inspected of inspectedUnits) {
                if (inspected.config.scratch?.platform !== "ps2") continue;
                let reports;
                try {
                  reports = await runJson(
                    python,
                    path.join(repo, "tools", "ps2-type-compare.py"),
                    ["-u", inspected.config.name, "--stdump", stdumpPath, "--types-file", requestFile, "--json"],
                    repo, signal,
                  );
                } catch (error) {
                  if (signal.aborted) throw error;
                  continue;
                }
                for (const report of reports) {
                  const previous = bestReports.get(report.type);
                  const reportScore = report.error ? -1 : report.match_percent;
                  const previousScore = !previous || previous.report.error
                    ? -1
                    : previous.report.match_percent;
                  if (!previous || reportScore > previousScore) {
                    bestReports.set(report.type, {
                      report,
                      unit: inspected.config.name,
                    });
                  }
                }
              }
            } finally {
              await fs.promises.unlink(requestFile).catch(() => {});
            }
            for (const typeName of typeNames) {
              const selected = bestReports.get(typeName);
              const report = selected?.report;
              const error = report?.error || (!report ? "type not found in any candidate unit" : undefined);
              annotations.push({
                kind: "type",
                platform,
                unit: selected?.unit || unit,
                type: typeName,
                typePercent: error ? null : report?.match_percent ?? null,
                error,
                comparisonDiff:
                  error || !report?.diff_lines?.length
                    ? undefined
                    : report.diff_lines.join("\n") + "\n",
              });
            }
          }
          if (!annotations.length) {
            throw new Error(objdiffEnabled
              ? "No objdiff functions or struct/class definitions were found in this file."
              : "No struct/class definitions were found in this PS2 file.");
          }
          const result = { unit, platform, candidateKey: cacheKey, annotations };
          if (signal.aborted) throw new vscode.CancellationError();
          await cache.write(repo, document.uri.fsPath, result);
          if (signal.aborted) throw new vscode.CancellationError();
          const count = await applyResult(document, result);
          if (force) vscode.window.showInformationMessage(`Updated ${count} PS2 match annotations.`);
          return;
        }

        const requestFile = path.join(os.tmpdir(), `nfsmw-functions-${process.pid}-${Date.now()}.json`);
        await fs.promises.writeFile(requestFile, JSON.stringify(functionNames), "utf8");
        let dwarfReports;
        try {
          if (objdiffEnabled) {
            dwarfReports = await runJson(
              python,
              path.join(repo, "tools", "dwarf-compare.py"),
              ["-u", unit, "--functions-file", requestFile, "--json"],
              repo, signal,
            );
          } else {
            let best;
            for (const inspected of inspectedUnits) {
              if (inspected.config.scratch?.platform === "ps2") continue;
              if (signal.aborted) throw new vscode.CancellationError();
              let reports;
              try {
                reports = await runJson(
                  python,
                  path.join(repo, "tools", "dwarf-compare.py"),
                  ["-u", inspected.config.name, "--functions-file", requestFile, "--json"],
                  repo, signal,
                );
              } catch (error) {
                if (signal.aborted) throw error;
                continue;
              }
              const score = reports.filter((report) => !report.error).length;
              if (!best || score > best.score) best = { reports, score, unit: inspected.config.name };
            }
            if (!best) throw new Error("DWARF comparison failed for every candidate unit.");
            dwarfReports = best.reports;
            unit = best.unit;
          }
        } finally {
          await fs.promises.unlink(requestFile).catch(() => {});
        }
        const dwarfByFunction = new Map(dwarfReports.map((report) => [report.function, report]));
        const annotationRows = objdiffEnabled
          ? sourceRows
          : dwarfReports.filter((report) => !report.error).map((report) => ({ name: report.function, match_percent: null }));
        const annotations = annotationRows.map((row) => {
          const dwarf = dwarfByFunction.get(row.name);
          const error = dwarf?.error;
          return {
            kind: "function",
            platform,
            unit,
            function: row.name,
            objdiffPercent: row.match_percent,
            dwarfPercent: error ? null : dwarf?.match_percent ?? null,
            error,
            dwarfDiff:
              error || !dwarf?.diff_lines?.length
                ? undefined
                : dwarf.diff_lines.join("\n") + "\n",
          };
        });
        const result = { unit, platform, candidateKey: cacheKey, annotations };
        if (signal.aborted) throw new vscode.CancellationError();
        await cache.write(repo, document.uri.fsPath, result);
        if (signal.aborted) throw new vscode.CancellationError();
        const count = await applyResult(document, result);
        if (force) vscode.window.showInformationMessage(`Updated ${count} function match annotations.`);
      },
    );
  }

  async function refresh(document, force) {
    const key = document.uri.toString();
    if (runningRefreshes.has(key)) return;
    const controller = new AbortController();
    runningRefreshes.set(key, controller);
    try {
      await refreshOnce(document, force, controller.signal, () => controller.abort());
    } finally {
      if (runningRefreshes.get(key) === controller) runningRefreshes.delete(key);
    }
  }

  context.subscriptions.push(
    vscode.commands.registerCommand("nfsmwMatch.refreshCurrentFile", async () => {
      const document = vscode.window.activeTextEditor?.document;
      if (!document) return;
      const key = document.uri.toString();
      const running = runningRefreshes.get(key);
      if (running) {
        running.abort();
        runningRefreshes.delete(key);
      }
      try {
        await refresh(document, true);
      } catch (error) {
        if (error instanceof vscode.CancellationError || error?.name === "AbortError") return;
        vscode.window.showErrorMessage(`NFSMW match refresh failed: ${error.message}`);
      }
    }),
    vscode.commands.registerCommand("nfsmwMatch.enableObjdiff", async () => {
      await vscode.workspace.getConfiguration("nfsmwMatch").update("objdiffEnabled", true, vscode.ConfigurationTarget.Workspace);
      vscode.window.showInformationMessage("NFSMW objdiff annotations enabled.");
    }),
    vscode.commands.registerCommand("nfsmwMatch.disableObjdiff", async () => {
      await vscode.workspace.getConfiguration("nfsmwMatch").update("objdiffEnabled", false, vscode.ConfigurationTarget.Workspace);
      vscode.window.showInformationMessage("NFSMW objdiff annotations disabled. Refresh a file to show only DWARF or PS2 type metrics.");
    }),
    vscode.commands.registerCommand("nfsmwMatch.setStdumpPath", async () => {
      try {
        const selected = await selectStdumpPath(true);
        vscode.window.showInformationMessage(`PS2 stdump path set to ${selected}`);
      } catch (error) {
        if (!String(error.message).includes("is required")) {
          vscode.window.showErrorMessage(`Failed to set stdump path: ${error.message}`);
        }
      }
    }),
    vscode.commands.registerCommand("nfsmwMatch.showDwarfDiff", async (annotation) => {
      if (!annotation?.dwarfDiff && !annotation?.comparisonDiff) return;
      const visibleDiff = vscode.window.visibleTextEditors.find(
        (editor) => editor.document.uri.scheme === "nfsmw-dwarf-diff",
      );
      const targetColumn = visibleDiff?.viewColumn || lastDiffViewColumn || vscode.ViewColumn.Beside;
      const document = await vscode.workspace.openTextDocument(diffs.uriFor(annotation));
      const editor = await vscode.window.showTextDocument(document, {
        preview: true,
        viewColumn: targetColumn,
      });
      lastDiffViewColumn = editor.viewColumn;
    }),
    vscode.window.onDidChangeVisibleTextEditors((editors) => {
      const visibleDiff = editors.find(
        (editor) => editor.document.uri.scheme === "nfsmw-dwarf-diff",
      );
      if (visibleDiff?.viewColumn) lastDiffViewColumn = visibleDiff.viewColumn;
    }),
    vscode.workspace.onDidOpenTextDocument((document) => {
      if (document.languageId === "c" || document.languageId === "cpp") refresh(document, false).catch(() => {});
    }),
    vscode.workspace.onDidChangeTextDocument((event) => {
      const document = event.document;
      if (document.languageId !== "c" && document.languageId !== "cpp") return;
      lenses.clear(document.uri);
      diagnostics.delete(document.uri);
      const key = document.uri.toString();
      const running = runningRefreshes.get(key);
      if (running) {
        running.abort();
        runningRefreshes.delete(key);
      }
    }),
    vscode.workspace.onDidSaveTextDocument((document) => {
      if (document.languageId !== "c" && document.languageId !== "cpp") return;
      const result = lastResults.get(document.uri.toString());
      if (result) applyResult(document, result).catch(() => {});
    }),
  );

  for (const document of vscode.workspace.textDocuments) {
    if (document.languageId === "c" || document.languageId === "cpp") refresh(document, false).catch(() => {});
  }
  await cache.prune();
}

function deactivate() {}

module.exports = { activate, deactivate };
