import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionManager;
import ghidra.program.model.symbol.Reference;
import ghidra.program.model.symbol.ReferenceIterator;
import java.io.PrintWriter;
import java.util.HashSet;

public class ExportWalls extends GhidraScript {
    private PrintWriter w;

    private String fnName(Address a) {
        Function f = getFunctionAt(a);
        if (f != null) {
            return f.getName() + "@" + f.getEntryPoint();
        }
        return "thunk/ext@" + a;
    }

    private void dumpWall(String addrStr) {
        try {
            Address a = currentProgram.getAddressFactory().getDefaultAddressSpace()
                    .getAddress(addrStr);
            Function f = getFunctionAt(a);
            if (f == null) {
                f = currentProgram.getFunctionManager().getFunctionAt(a);
            }
            if (f == null) {
                w.println("WALL " + addrStr + " NO_FUNCTION");
                return;
            }
            HashSet<String> callers = new HashSet<String>();
            for (Function c : f.getCallingFunctions(monitor)) {
                callers.add(c.getName() + "@" + c.getEntryPoint());
            }
            HashSet<String> callees = new HashSet<String>();
            for (Function c : f.getCalledFunctions(monitor)) {
                callees.add(c.getName() + "@" + c.getEntryPoint());
            }
            int dataIn = 0, dataOut = 0;
            ReferenceIterator ri = currentProgram.getReferenceManager()
                    .getReferencesTo(f.getEntryPoint());
            while (ri.hasNext()) {
                Reference r = ri.next();
                if (!r.getReferenceType().isCall() && !r.getReferenceType().isJump()) {
                    dataIn++;
                }
            }
            w.println("WALL " + addrStr + " fn=" + f.getName() + " bodyBytes="
                    + f.getBody().getNumAddresses() + " callers=" + callers.size()
                    + " callees=" + callees.size() + " dataRefsIn=" + dataIn);
            int n = 0;
            for (String c : callers) {
                if (n++ >= 12) {
                    w.println("    caller... (+" + (callers.size() - 12) + " more)");
                    break;
                }
                w.println("    caller " + c);
            }
            n = 0;
            for (String c : callees) {
                if (n++ >= 20) {
                    w.println("    callee... (+" + (callees.size() - 20) + " more)");
                    break;
                }
                w.println("    callee " + c);
            }
        }
        catch (Exception e) {
            w.println("WALL " + addrStr + " ERROR " + e.getMessage());
        }
    }

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        String out = args.length > 0 ? args[0]
                : "C:\\Users\\jferr\\AppData\\Local\\Temp\\opencode\\ghidra_walls.txt";
        w = new PrintWriter(out, "UTF-8");
        FunctionManager fm = currentProgram.getFunctionManager();
        w.println("== TOTAL_FUNCTIONS " + fm.getFunctionCount());
        String[] walls = {"8007c390", "8008095c", "802feab0", "802cb51c", "8008f098",
                "80109358", "80109bb8", "8026c574", "800d80fc", "80075c44", "80321474",
                "802c9fb8", "8024b310", "8030183c", "803598fc", "802bc0a4", "802dfcac",
                "8034bac4", "802a2248", "8031f7fc", "8034bd3c", "800a3090", "80321330",
                "802dd360", "80320840", "8010ac40", "8031fa48", "803210b8"};
        for (String s : walls) {
            dumpWall(s);
        }
        w.close();
        println("== WALLS EXPORTED to " + out);
    }
}
