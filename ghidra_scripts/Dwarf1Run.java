// Lanza el DWARF1ProgramAnalyzer de la extension ghidra-dwarf1 (parcheada para
// los seis atributos GNU 0x8000..0x8050) por REFLEXION: el analizador viene con
// setDefaultEnablement(false) y el headless no lo dispara solo.
//
// Escribe un censo antes/despues para poder afirmar que importo de verdad.
//@category NFSMW

import ghidra.app.script.GhidraScript;
import ghidra.app.util.importer.MessageLog;
import ghidra.program.model.data.DataTypeManager;
import ghidra.program.model.data.Structure;
import ghidra.program.model.data.Union;
import ghidra.program.model.data.DataType;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.symbol.SourceType;
import java.io.PrintWriter;
import java.util.Iterator;

public class Dwarf1Run extends GhidraScript {

    private PrintWriter w;

    private void censo(String tag) {
        int nfun = 0, sigImported = 0, withParams = 0;
        FunctionIterator it = currentProgram.getFunctionManager().getFunctions(true);
        while (it.hasNext()) {
            Function f = it.next();
            nfun++;
            if (f.getSignatureSource() == SourceType.IMPORTED) {
                sigImported++;
            }
            if (f.getParameterCount() > 0) {
                withParams++;
            }
        }
        DataTypeManager dtm = currentProgram.getDataTypeManager();
        int nstruct = 0, conCampos = 0, nunion = 0, nenum = 0;
        Iterator<Structure> sit = dtm.getAllStructures();
        while (sit.hasNext()) {
            Structure s = sit.next();
            nstruct++;
            if (s.getNumDefinedComponents() > 0) {
                conCampos++;
            }
        }
        Iterator<DataType> dit = dtm.getAllDataTypes();
        while (dit.hasNext()) {
            DataType dt = dit.next();
            if (dt instanceof Union) {
                nunion++;
            }
            else if (dt instanceof ghidra.program.model.data.Enum) {
                nenum++;
            }
        }
        w.println("CENSO " + tag);
        w.println("  funciones                : " + nfun);
        w.println("  firma == IMPORTED        : " + sigImported);
        w.println("  con >=1 parametro        : " + withParams);
        w.println("  structs                  : " + nstruct + " (con campos: " + conCampos + ")");
        w.println("  unions                   : " + nunion);
        w.println("  enums                    : " + nenum);
        w.println("  tipos en el DTM          : " + dtm.getDataTypeCount(true));
        w.flush();
    }

    @Override
    public void run() throws Exception {
        String out = getScriptArgs().length > 0 ? getScriptArgs()[0] : "dwarf1run.txt";
        w = new PrintWriter(out, "UTF-8");
        w.println("programa  : " + currentProgram.getName());
        w.println("lenguaje  : " + currentProgram.getLanguageID());
        w.println("ghidra    : " + ghidra.framework.Application.getApplicationVersion());
        w.println();
        censo("ANTES");
        w.println();

        long t0 = System.currentTimeMillis();
        MessageLog log = new MessageLog();
        boolean ok;
        try {
            Class<?> cls = Class.forName("com.github.rafalh.ghidra.dwarfone.DWARF1ProgramAnalyzer");
            Object pa = cls.getConstructors()[0].newInstance(currentProgram,
                    currentProgram.getMemory(), monitor, log);
            ok = (Boolean) cls.getMethod("process").invoke(pa);
        }
        catch (Throwable e) {
            Throwable c = e.getCause() != null ? e.getCause() : e;
            w.println("EXCEPCION AL ANALIZAR: " + c);
            for (StackTraceElement s : c.getStackTrace()) {
                w.println("    at " + s);
            }
            ok = false;
        }
        w.println("process() = " + ok + "  en " + (System.currentTimeMillis() - t0) / 1000 + " s");
        String lg = log.toString();
        w.println("MessageLog: " + lg.length() + " chars (primeros 4000)");
        w.println(lg.length() > 4000 ? lg.substring(0, 4000) : lg);
        w.println();
        censo("DESPUES");
        w.close();
        println("DWARF1 process() = " + ok);
    }
}
