// Decompiles functions by name or address and writes the C output to a file.
// Arguments: <output.c> <name | 0xaddress>...
// A name matches every function with that name, in any namespace ("Class::Method" also works),
// and every GCC 2 mangled function whose name starts with "<name>__".
// @category NFSMW
import java.io.FileOutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.List;

import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionManager;
import ghidra.program.model.symbol.Symbol;

public class DecompilarFunciones extends GhidraScript {

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length < 2) {
            printerr("usage: <output.c> <name | 0xaddress>...");
            return;
        }
        DecompInterface decompiler = new DecompInterface();
        decompiler.openProgram(currentProgram);
        try (PrintWriter out = new PrintWriter(new OutputStreamWriter(new FileOutputStream(args[0]), "UTF-8"))) {
            for (int i = 1; i < args.length; i++) {
                List<Function> functions = lookup(args[i]);
                if (functions.isEmpty()) {
                    out.println("// NOT FOUND: " + args[i]);
                    continue;
                }
                for (Function f : functions) {
                    DecompileResults r = decompiler.decompileFunction(f, 120, monitor);
                    out.println("// " + f.getName(true) + " @ " + f.getEntryPoint() + " (" + f.getBody().getNumAddresses() + " bytes)");
                    if (r != null && r.decompileCompleted()) {
                        out.println(r.getDecompiledFunction().getC());
                    } else {
                        out.println("// FAILED: " + (r == null ? "no result" : r.getErrorMessage()));
                    }
                }
            }
        } finally {
            decompiler.dispose();
        }
    }

    private List<Function> lookup(String query) {
        List<Function> found = new ArrayList<>();
        FunctionManager fm = currentProgram.getFunctionManager();
        if (query.startsWith("0x")) {
            Address a = toAddr(Long.parseUnsignedLong(query.substring(2), 16));
            Function f = fm.getFunctionContaining(a);
            if (f != null) {
                found.add(f);
            }
            return found;
        }
        for (Symbol s : currentProgram.getSymbolTable().getSymbols(query)) {
            Function f = fm.getFunctionAt(s.getAddress());
            if (f != null && !found.contains(f)) {
                found.add(f);
            }
        }
        if (found.isEmpty()) {
            // Also the GCC 2 mangled forms of a plain name: name__<signature>.
            for (Function f : fm.getFunctions(true)) {
                String name = f.getName(true);
                if (name.equals(query) || f.getName().startsWith(query + "__")) {
                    found.add(f);
                }
            }
        }
        return found;
    }
}
