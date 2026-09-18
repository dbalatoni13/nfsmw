import ghidra.app.script.GhidraScript;
import ghidra.program.model.mem.MemoryBlock;
import ghidra.program.model.symbol.Symbol;
import ghidra.program.model.symbol.SymbolIterator;
import ghidra.program.model.symbol.SymbolTable;

public class DumpInfo extends GhidraScript {
    @Override
    public void run() throws Exception {
        println("== PROGRAM: " + currentProgram.getName());
        println("== LANGUAGE: " + currentProgram.getLanguage().getLanguageID());
        println("== COMPILER: " + currentProgram.getCompilerSpec().getCompilerSpecID());
        println("== IMAGEBASE: " + currentProgram.getImageBase());
        println("== EXEFORMAT: " + currentProgram.getExecutableFormat());
        println("== BLOCKS: " + currentProgram.getMemory().getBlocks().length);
        for (MemoryBlock b : currentProgram.getMemory().getBlocks()) {
            println(String.format("BLOCK %-24s start=%s end=%s size=%d r=%b w=%b x=%b init=%b",
                    b.getName(), b.getStart(), b.getEnd(), b.getSize(),
                    b.isRead(), b.isWrite(), b.isExecute(), b.isInitialized()));
        }
        SymbolTable st = currentProgram.getSymbolTable();
        println("== NUMSYMBOLS: " + st.getNumSymbols());
        println("== NUMFUNCTIONS: " + currentProgram.getFunctionManager().getFunctionCount());
        int n = 0;
        SymbolIterator it = st.getAllSymbols(false);
        while (it.hasNext() && n < 60) {
            Symbol s = it.next();
            println("SYM " + s.getName() + " @ " + s.getAddress()
                    + " src=" + s.getSource() + " ext=" + s.isExternal());
            n++;
        }
        println("== DONE");
    }
}
