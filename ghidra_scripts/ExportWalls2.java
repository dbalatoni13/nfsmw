import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.listing.FunctionManager;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.InstructionIterator;
import ghidra.program.model.symbol.Reference;
import ghidra.program.model.symbol.ReferenceIterator;
import ghidra.program.model.symbol.Symbol;
import java.io.PrintWriter;

public class ExportWalls2 extends GhidraScript {
    private PrintWriter w;
    private FunctionManager fm;

    private Address addr(String s) {
        try {
            return currentProgram.getAddressFactory().getDefaultAddressSpace()
                    .getAddress(s);
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }
    }

    private String hexPlus(String base, int off) {
        return String.format("%08x", Long.parseLong(base, 16) + off);
    }

    private void dumpDetail(String aStr, int expect) {
        Address a = addr(aStr);
        Function f = fm.getFunctionAt(a);
        w.println("===== DETAIL " + aStr + " expect=" + expect);
        if (f == null) {
            w.println("  NO_FUNCTION_AT_ENTRY");
            return;
        }
        w.println("  fn=" + f.getName() + " entry=" + f.getEntryPoint()
                + " body=[" + f.getBody().getMinAddress() + " .. "
                + f.getBody().getMaxAddress() + "]"
                + " naddr=" + f.getBody().getNumAddresses()
                + " thunk=" + f.isThunk() + " external=" + f.isExternal()
                + " hasVarArgs=" + f.hasVarArgs());
        if (f.isThunk()) {
            try {
                w.println("  thunked=" + f.getThunkedFunction(true));
            }
            catch (Exception e) {
                w.println("  thunked=ERROR " + e.getMessage());
            }
        }
        w.println("  -- first instructions:");
        try {
            InstructionIterator ii = currentProgram.getListing()
                    .getInstructions(f.getBody(), true);
            int n = 0;
            while (ii.hasNext() && n < 8) {
                Instruction ins = ii.next();
                w.println("    " + ins.getAddress() + "  " + ins.toString());
                n++;
            }
        }
        catch (Exception e) {
            w.println("    INSN_ERROR " + e.getMessage());
        }
        w.println("  -- refs TO entry (max 20):");
        ReferenceIterator ri = currentProgram.getReferenceManager().getReferencesTo(a);
        int m = 0;
        while (ri.hasNext() && m < 20) {
            Reference r = ri.next();
            Address from = r.getFromAddress();
            Function cf = fm.getFunctionContaining(from);
            Symbol s = currentProgram.getSymbolTable().getPrimarySymbol(from);
            w.println("    from=" + from + " type=" + r.getReferenceType()
                    + " inFn=" + (cf == null ? "-" : cf.getName() + "@" + cf.getEntryPoint())
                    + " sym=" + (s == null ? "-" : s.getName()));
            m++;
        }
        w.println("  -- functions with entry inside [entry, entry+expect) (max 25):");
        Address end = addr(hexPlus(aStr, expect));
        FunctionIterator fi = fm.getFunctions(true);
        int k = 0;
        while (fi.hasNext()) {
            Function g = fi.next();
            Address e = g.getEntryPoint();
            if (e.compareTo(a) > 0 && e.compareTo(end) < 0) {
                w.println("    inner fn=" + g.getName() + " entry=" + e
                        + " bodyN=" + g.getBody().getNumAddresses());
                if (++k >= 25) {
                    w.println("    ... truncated");
                    break;
                }
            }
        }
        if (k == 0) {
            w.println("    (none)");
        }
        Function mid = fm.getFunctionContaining(addr(hexPlus(aStr, expect / 2)));
        w.println("  -- function containing midpoint(" + hexPlus(aStr, expect / 2) + "): "
                + (mid == null ? "-" : mid.getName() + "@" + mid.getEntryPoint()
                        + " bodyN=" + mid.getBody().getNumAddresses()));
    }

    private void decompileAt(String label, String aStr, int expect) {
        Function f = fm.getFunctionAt(addr(aStr));
        if (f == null || f.getBody().getNumAddresses() < 100) {
            Function mid = fm.getFunctionContaining(addr(hexPlus(aStr, expect / 2)));
            if (mid != null && mid.getBody().getNumAddresses() >= 100) {
                f = mid;
            }
        }
        w.println("===== DECOMP " + label + " (used " + (f == null ? "NONE"
                : f.getName() + "@" + f.getEntryPoint()) + ")");
        if (f == null) {
            w.println("  NO_FUNCTION");
            return;
        }
        DecompInterface dec = new DecompInterface();
        try {
            dec.openProgram(currentProgram);
            DecompileResults res = dec.decompileFunction(f, 180, monitor);
            if (res != null && res.decompileCompleted()) {
                w.println(res.getDecompiledFunction().getC());
            }
            else {
                w.println("  DECOMPILE_FAILED "
                        + (res == null ? "null" : res.getErrorMessage()));
            }
        }
        catch (Exception e) {
            w.println("  DECOMPILE_ERROR " + e.getMessage());
        }
        finally {
            dec.dispose();
        }
    }

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        String out = args.length > 0 ? args[0]
                : "C:\\Users\\jferr\\AppData\\Local\\Temp\\opencode\\ghidra_walls2.txt";
        fm = currentProgram.getFunctionManager();
        w = new PrintWriter(out, "UTF-8");
        String[][] tinies = {{"8007c390", "3868"}, {"8008095c", "3604"},
                {"802feab0", "2980"}, {"802cb51c", "2908"}, {"80109358", "2072"},
                {"80109bb8", "2044"}, {"8026c574", "1588"}, {"800d80fc", "1240"},
                {"80075c44", "992"}, {"802c9fb8", "876"}, {"8024b310", "856"},
                {"8030183c", "816"}, {"802bc0a4", "708"}};
        for (String[] t : tinies) {
            dumpDetail(t[0], Integer.parseInt(t[1]));
        }
        decompileAt("Setup_RoadblockFlow", "802a2248", 596);
        decompileAt("Effect_Init", "80320840", 276);
        decompileAt("GenerateRoadNoise", "800d80fc", 1240);
        decompileAt("static_init_zCamera", "8008095c", 3604);
        w.close();
        println("== WALLS2 EXPORTED to " + out);
    }
}
