import ghidra.app.script.GhidraScript;
import ghidra.program.model.mem.MemoryBlock;
import ghidra.program.model.symbol.Symbol;
import ghidra.program.model.symbol.SymbolIterator;
import java.io.PrintWriter;

public class ExportSymbols extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        String out = args.length > 0 ? args[0]
                : "C:\\Users\\jferr\\AppData\\Local\\Temp\\opencode\\ghidra_symbols.csv";
        PrintWriter w = new PrintWriter(out, "UTF-8");
        w.println("addr,name,source,external,exec");
        SymbolIterator it = currentProgram.getSymbolTable().getAllSymbols(false);
        int n = 0;
        while (it.hasNext()) {
            Symbol s = it.next();
            boolean exec = false;
            MemoryBlock b = currentProgram.getMemory().getBlock(s.getAddress());
            if (b != null) {
                exec = b.isExecute();
            }
            String name = s.getName().replace(',', ';').replace('"', '\'');
            w.println(s.getAddress() + "," + name + "," + s.getSource() + "," + s.isExternal()
                    + "," + exec);
            n++;
        }
        w.close();
        println("== EXPORTED " + n + " symbols to " + out);
    }
}
