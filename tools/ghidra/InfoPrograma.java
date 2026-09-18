// Writes a summary of the open program: language, image base, memory blocks, function count,
// and the functions whose name contains each given substring (up to 50 per substring).
// Arguments: <output.txt> [substring]...
// @category NFSMW
import java.io.FileOutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;

import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.mem.MemoryBlock;

public class InfoPrograma extends GhidraScript {

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        try (PrintWriter out = new PrintWriter(new OutputStreamWriter(new FileOutputStream(args[0]), "UTF-8"))) {
            out.println("program: " + currentProgram.getName());
            out.println("language: " + currentProgram.getLanguageID() + " / " + currentProgram.getCompilerSpec().getCompilerSpecID());
            out.println("image base: " + currentProgram.getImageBase());
            for (MemoryBlock b : currentProgram.getMemory().getBlocks()) {
                out.println("  block " + b.getName() + " " + b.getStart() + "-" + b.getEnd());
            }
            out.println("functions: " + currentProgram.getFunctionManager().getFunctionCount());
            int shown = 0;
            for (Function f : currentProgram.getFunctionManager().getFunctions(true)) {
                if (shown++ >= 5) {
                    break;
                }
                out.println("  first: " + f.getName(true) + " @ " + f.getEntryPoint());
            }
            for (int i = 1; i < args.length; i++) {
                int n = 0;
                for (Function f : currentProgram.getFunctionManager().getFunctions(true)) {
                    if (f.getName(true).contains(args[i])) {
                        out.println("  [" + args[i] + "] " + f.getName(true) + " @ " + f.getEntryPoint());
                        if (++n >= 50) {
                            break;
                        }
                    }
                }
                out.println("  [" + args[i] + "] " + n + " shown");
            }
        }
    }
}
