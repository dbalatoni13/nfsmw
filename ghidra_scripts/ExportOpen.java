// Exporta el decompilado de una lista de funciones (addr,tam,etiqueta) en un
// proyecto que YA tiene las dos piezas puestas: sleigh Gekko/Broadway (para que
// los paired-singles no trunquen) y la extension DWARF1 (para que salgan tipos y
// nombres de miembro en vez de param_1 + 0x24).
//
//   -postScript ExportOpen.java <lista.csv> <salida.c> <salida.asm> <resumen.txt>
//
// Ademas del C escribe:
//   - el LISTADO de instrucciones con la palabra en hex (sin el, cualquier
//     conclusion sobre && contra || es adivinanza: el decompilador normaliza la
//     forma booleana y parte un `or.` de siete terminos en siete &&),
//   - un resumen con cobertura, numero de paired-singles y si la firma viene del
//     DWARF (IMPORTED) o es inventada por Ghidra (DEFAULT).
//@category NFSMW

import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileOptions;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.address.AddressSet;
import ghidra.program.model.lang.Register;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.InstructionIterator;
import ghidra.program.model.listing.Listing;
import ghidra.program.model.symbol.SourceType;
import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.math.BigInteger;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.List;

public class ExportOpen extends GhidraScript {

    private PrintWriter c;
    private PrintWriter asm;
    private PrintWriter sum;
    private DecompInterface dec;
    private Listing lst;

    private Address ad(String s) {
        return currentProgram.getAddressFactory().getDefaultAddressSpace()
                .getAddress(Long.parseLong(s.trim().replaceFirst("^0[xX]", ""), 16));
    }

    private Function pick(Address a, int expect) {
        Function f = currentProgram.getFunctionManager().getFunctionAt(a);
        if (f == null) {
            f = currentProgram.getFunctionManager().getFunctionContaining(a.add(expect / 2));
        }
        return f;
    }

    private void one(String aStr, int expect, String label) throws Exception {
        Address a = ad(aStr);
        Function f = pick(a, expect);
        if (f == null) {
            f = createFunction(a, label);
        }
        int insn = 0, bytes = 0, ps = 0;
        String firstGap = "-";
        for (long p = Long.parseLong(aStr, 16); p < Long.parseLong(aStr, 16) + expect; p += 4) {
            Address x = currentProgram.getAddressFactory().getDefaultAddressSpace().getAddress(p);
            Instruction in = lst.getInstructionAt(x);
            if (in == null) {
                firstGap = String.format("%08x", p);
                break;
            }
        }
        AddressSet body = new AddressSet(a, a.add(expect - 1));
        InstructionIterator ii = lst.getInstructions(body, true);
        while (ii.hasNext()) {
            Instruction in = ii.next();
            insn++;
            bytes += in.getLength();
            String m = in.getMnemonicString();
            if (m.startsWith("ps_") || m.startsWith("psq_")) {
                ps++;
            }
        }
        String proto = (f == null) ? "<sin funcion>" : f.getPrototypeString(true, false);
        String src = (f == null) ? "-" : f.getSignatureSource().toString();
        sum.println(String.format("%-9s %-6d %-6d %-7.1f %-5s %-9s %-9s %s",
                aStr, expect, insn, 100.0 * bytes / expect, ps, firstGap, src, label));
        sum.flush();

        c.println("/* ================================================================== */");
        c.println("/* " + aStr + "  " + label);
        c.println("   tam=" + expect + " B  insn=" + insn + "  cobertura=" + bytes + "/" + expect
                + "  paired-singles=" + ps + "  firma=" + src);
        c.println("   proto: " + proto);
        c.println("*/");
        if (f == null) {
            c.println("/* NO HAY FUNCION EN " + aStr + " */");
            c.println();
            return;
        }
        try {
            DecompileResults r = dec.decompileFunction(f, 600, monitor);
            if (r != null && r.decompileCompleted() && r.getDecompiledFunction() != null) {
                c.println(r.getDecompiledFunction().getC());
            }
            else {
                c.println("/* DECOMPILE_FAILED: " + (r == null ? "null" : r.getErrorMessage()) + " */");
            }
        }
        catch (Exception e) {
            c.println("/* DECOMPILE_ERROR " + e + " */");
        }
        c.println();
        c.flush();

        asm.println("===== ASM " + label + "  " + aStr + "  (" + expect + " B)");
        ii = lst.getInstructions(body, true);
        while (ii.hasNext()) {
            Instruction in = ii.next();
            StringBuilder raw = new StringBuilder();
            try {
                for (byte b : in.getBytes()) {
                    raw.append(String.format("%02x", b & 0xff));
                }
            }
            catch (Exception e) {
                raw.append("????????");
            }
            asm.println("  " + in.getAddress() + "  " + raw + "  " + in);
        }
        asm.println();
        asm.flush();
    }

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length < 4) {
            println("uso: ExportOpen.java <lista.csv> <salida.c> <salida.asm> <resumen.txt>");
            return;
        }
        lst = currentProgram.getListing();

        Register gqr0 = currentProgram.getLanguage().getRegister("GQR0");
        String gqrNote = "GQR0 no existe en este lenguaje";
        if (gqr0 != null) {
            BigInteger v = currentProgram.getProgramContext().getValue(gqr0, ad("80109364"), false);
            gqrNote = "GQR0 en 80109364 = " + (v == null ? "<sin fijar>" : v.toString());
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
            jobs.add(new String[] {p[0].trim(), p[1].trim(), p.length > 2 ? p[2].trim() : p[0].trim()});
        }
        r.close();

        c = new PrintWriter(args[1], "UTF-8");
        asm = new PrintWriter(args[2], "UTF-8");
        sum = new PrintWriter(args[3], "UTF-8");
        dec = new DecompInterface();
        dec.setOptions(new DecompileOptions());
        dec.toggleCCode(true);
        dec.toggleSyntaxTree(true);
        dec.setSimplificationStyle("decompile");

        sum.println("LANGUAGE = " + currentProgram.getLanguageID());
        sum.println("GHIDRA   = " + ghidra.framework.Application.getApplicationVersion());
        sum.println(gqrNote);
        sum.println();
        sum.println(String.format("%-9s %-6s %-6s %-7s %-5s %-9s %-9s %s",
                "addr", "tam", "insn", "cover%", "ps", "hueco", "firma", "nombre"));

        c.println("/* Decompilado de las funciones abiertas de la r63.");
        c.println("   " + currentProgram.getLanguageID() + " + extension DWARF1 (ghidra-dwarf1 parcheada).");
        c.println("   " + gqrNote + "  */");
        c.println();

        try {
            dec.openProgram(currentProgram);
            for (String[] j : jobs) {
                println("decompilando " + j[2] + " @" + j[0]);
                one(j[0], Integer.parseInt(j[1]), j[2]);
            }
        }
        finally {
            dec.dispose();
            c.close();
            asm.close();
            sum.close();
        }
        println("== EXPORTADAS " + jobs.size());
    }
}
