// Fija GQR0 = 0 en todo el rango de codigo ANTES del auto-analisis.
//
// Medido por el agente "gekko" de la r63: en NFSMW solo hay 5 escrituras a GQR0
// (80003304, 8030f8a4, 8030fb5c, 8032226c = "li rN,0; mtspr GQR0,rN"; 8030f930 es
// la recarga de contexto FPU del sistema de un valor que ya era 0). Sin fijarlo,
// cada psq_l/psq_st se expande en 15 lineas de if sobre el tipo de cuantizacion.
// GQR1..GQR7 NO se fijan: ahi la cuantizacion es real (269 accesos).
//@category NFSMW

import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.lang.Register;
import ghidra.program.model.mem.MemoryBlock;
import java.math.BigInteger;

public class SetGqr extends GhidraScript {
    @Override
    public void run() throws Exception {
        Register gqr0 = currentProgram.getLanguage().getRegister("GQR0");
        println("LANGUAGE = " + currentProgram.getLanguageID());
        if (gqr0 == null) {
            println("SETGQR: NO HAY REGISTRO GQR0 EN ESTE LENGUAJE -- no se fija nada");
            return;
        }
        int n = 0;
        for (MemoryBlock b : currentProgram.getMemory().getBlocks()) {
            if (!b.isExecute()) {
                continue;
            }
            Address s = b.getStart();
            Address e = b.getEnd();
            currentProgram.getProgramContext().setValue(gqr0, s, e, BigInteger.ZERO);
            println("SETGQR: GQR0=0 en " + b.getName() + " [" + s + " .. " + e + "]");
            n++;
        }
        println("SETGQR: bloques ejecutables tratados = " + n);
    }
}
