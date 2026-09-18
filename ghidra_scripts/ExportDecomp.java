// Decompila en bloque una lista de funciones leida de un fichero.
//
//   analyzeHeadless <projdir> <projname> -process <prog> -noanalysis \
//       -scriptPath <scripts> -postScript ExportDecomp.java <lista> <salida>
//
// <lista>: una funcion por linea, "addr,tam,etiqueta". Lineas vacias y las que
// empiezan por '#' se ignoran. Ejemplo:
//
//   802a2248,596,Setup__Q26Speech13RoadblockFlow
//   80320840,276,Effect_Init
//
// Por cada entrada escribe en <salida> una cabecera "===== DECOMP", el detalle
// del cuerpo que Ghidra reconoce (para delatar los truncados por paired-singles)
// y el C del decompilador.
//
// Un tercer argumento opcional recibe el LISTADO de instrucciones. Hace falta:
// el decompilador NORMALIZA la forma booleana -- un `or.` de siete terminos sale
// impreso como una cadena de siete `&&`. Sin el listado, cualquier conclusion
// sobre `&&` contra `||` es adivinanza.
//
// Sustituye a las direcciones a fuego de ExportWalls2.java.
//@category NFSMW

import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionManager;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.InstructionIterator;
import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.List;

public class ExportDecomp extends GhidraScript {
    private PrintWriter w;
    private PrintWriter asm;
    private FunctionManager fm;
    private DecompInterface dec;

    private Address addr(String s) {
        return currentProgram.getAddressFactory().getDefaultAddressSpace()
                .getAddress(Long.parseLong(s.trim().replaceFirst("^0[xX]", ""), 16));
    }

    /** La funcion que de verdad cubre [a, a+expect): si la entrada esta vacia o
     *  truncada, se prueba el punto medio, como hacia ExportWalls2. */
    private Function pick(Address a, int expect) {
        Function f = fm.getFunctionAt(a);
        if (f == null || f.getBody().getNumAddresses() < 100) {
            Address mid = a.add(expect / 2);
            Function g = fm.getFunctionContaining(mid);
            if (g != null && g.getBody().getNumAddresses() >= 100) {
                return g;
            }
        }
        return f;
    }

    private void one(String aStr, int expect, String label) {
        Address a = addr(aStr);
        Function f = pick(a, expect);
        w.println("===== DECOMP " + label + " addr=" + aStr + " expect=" + expect);
        if (f == null) {
            w.println("  NO_FUNCTION_AT " + aStr);
            w.println();
            return;
        }
        long n = f.getBody().getNumAddresses();
        w.println("  fn=" + f.getName() + " entry=" + f.getEntryPoint()
                + " body=[" + f.getBody().getMinAddress() + " .. "
                + f.getBody().getMaxAddress() + "] naddr=" + n
                + " coverage=" + n + "/" + expect
                + (n < expect ? "  *** TRUNCADO ***" : ""));
        try {
            DecompileResults res = dec.decompileFunction(f, 300, monitor);
            if (res != null && res.decompileCompleted()) {
                w.println(res.getDecompiledFunction().getC());
            }
            else {
                w.println("  DECOMPILE_FAILED "
                        + (res == null ? "null" : res.getErrorMessage()));
            }
        }
        catch (Exception e) {
            w.println("  DECOMPILE_ERROR " + e);
        }
        w.println();
        w.flush();
        if (asm != null) {
            asm.println("===== ASM " + label + " " + f.getName()
                    + " [" + f.getBody().getMinAddress() + " .. "
                    + f.getBody().getMaxAddress() + "]");
            InstructionIterator ii = currentProgram.getListing()
                    .getInstructions(f.getBody(), true);
            while (ii.hasNext()) {
                Instruction ins = ii.next();
                StringBuilder raw = new StringBuilder();
                try {
                    for (byte b : ins.getBytes()) {
                        raw.append(String.format("%02x", b & 0xff));
                    }
                }
                catch (Exception e) {
                    raw.append("????????");
                }
                asm.println("  " + ins.getAddress() + "  " + raw + "  " + ins);
            }
            asm.println();
            asm.flush();
        }
    }

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length < 2) {
            println("uso: ExportDecomp.java <lista.csv> <salida.txt> [<salida.asm>]");
            return;
        }
        List<String[]> jobs = new ArrayList<String[]>();
        BufferedReader r = new BufferedReader(new InputStreamReader(
                new FileInputStream(args[0]), StandardCharsets.UTF_8));
        String line;
        while ((line = r.readLine()) != null) {
            line = line.trim();
            if (line.isEmpty() || line.startsWith("#")) {
                continue;
            }
            String[] p = line.split(",", 3);
            if (p.length < 2) {
                println("linea ignorada: " + line);
                continue;
            }
            jobs.add(new String[] {p[0].trim(), p[1].trim(),
                    p.length > 2 ? p[2].trim() : p[0].trim()});
        }
        r.close();

        fm = currentProgram.getFunctionManager();
        dec = new DecompInterface();
        w = new PrintWriter(args[1], "UTF-8");
        asm = args.length > 2 ? new PrintWriter(args[2], "UTF-8") : null;
        try {
            dec.openProgram(currentProgram);
            for (String[] j : jobs) {
                println("decompilando " + j[2] + " @" + j[0]);
                one(j[0], Integer.parseInt(j[1]), j[2]);
            }
        }
        finally {
            dec.dispose();
            w.close();
            if (asm != null) {
                asm.close();
            }
        }
        println("== EXPORTADAS " + jobs.size() + " a " + args[1]);
    }
}
