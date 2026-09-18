cmake_minimum_required(VERSION 3.22)
project(nfsmw_probe C CXX)
# Fase 0/1: codigo portable del decomp compilado con clang/NDK arm64.
# CARP queda fuera por ahora (idiomas STLport: ver PORT-ANDROID.md).
get_filename_component(GAME_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../" ABSOLUTE)
add_library(nfsmw_probe SHARED
    "${GAME_ROOT}/src/Speed/Indep/Libs/Support/Utility/UMath.cpp"
    port_probe.cpp
    port_gl.cpp
    gx2d.cpp
    assets.cpp
    port_aaudio.cpp
    port_epoly.cpp
    port_feng.cpp
    port_lz.cpp
    port_font.cpp
    port_pixfont.cpp
    port_boot.cpp
    port_snd.cpp
    port_snd_hal.cpp
    platform_stubs.cpp
)
target_include_directories(nfsmw_probe PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/platform"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/include"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/mix"
    "${GAME_ROOT}/src/Speed/Indep/Libs/Support/Utility"
    "${GAME_ROOT}/src"
)
target_compile_options(nfsmw_probe PRIVATE -std=gnu++98 -fno-exceptions -fno-rtti -O2 -DM_TWOPI=6.283185307179586476925286766559f)
target_link_libraries(nfsmw_probe snd_ea android EGL GLESv2 log aaudio)
message(STATUS "GAME_ROOT = ${GAME_ROOT}")

add_executable(probe_main probe_main.cpp "${GAME_ROOT}/src/Speed/Indep/Libs/Support/Utility/UMath.cpp" platform_stubs.cpp)
target_include_directories(probe_main PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/platform"
    "${GAME_ROOT}/src/Speed/Indep/Libs/Support/Utility"
    "${GAME_ROOT}/src"
)
target_compile_options(probe_main PRIVATE -std=gnu++98 -fno-exceptions -fno-rtti -O2 -DM_TWOPI=6.283185307179586476925286766559f)

# Libreria snd de EA: las 135 unidades del decomp que clang/arm64 acepta, de 139.
# Es el mezclador REAL del juego, no una reimplementacion. Y de paso es un
# SEGUNDO COMPILADOR pasando por encima del arbol decompilado.
#
# Las 4 que quedan fuera, y ninguna es un fallo del port:
#   sdspmix.c, snddrv.c    el DSP de GameCube
#   mtdecppc.cpp, lbmpeg.cpp  llevan ensamblador PowerPC en linea
# Las cuatro son justo las que el backend sustituye.
add_library(snd_ea STATIC
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/SNDI_cos.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/SNDI_findprime.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/SNDI_mult16.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/SNDI_root1x.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/SNDI_sin.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/s3dlow.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/saems.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/saemsamb.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/saemsmbf.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/saemsmbm.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/saemstimupdt.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/salloc.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/satospkr.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/sattrdef.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/sbadd.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/sballoc.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/sbasync.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/sbasyncm.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/sbhdrcpy.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/sbhdrsze.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/sbpatinf.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/sbplay.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/sbremove.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/sbvalid.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/scheckpo.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/sclcptch.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/sclnt100.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/sctlfilt.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/sctrldry.cpp"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/sdata.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/sexithndl.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/sfxlevel.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/sgetdata.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/sgetpvol.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/sgettag.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/sgparse.cpp"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/slinklst.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/slowpass.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/smemcpy.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/smemdis.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/smemhigh.cpp"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/smemlmt.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/smemlu.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/smemman.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/sndfxbus.cpp"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/sover.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/spantoaz.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/spat2hdr.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/spatkey.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/spitch.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/spktctoh.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/spktplay.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/splysdef.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/spoutlat.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/sprofvoc.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/srandom.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/sreallocbuf.cpp"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/srender.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/sresopat.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/srrange.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/sserver.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/ssine.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/sst.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/sst3dpos.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/sstautov.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/sstcrtap.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/sstfxlev.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/sstgetpv.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/sstgetrp.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/ssthold.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/sstlowp.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/sstop.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/sstovrhd.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/sstpmult.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/sstqmem.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/sstqreqi.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/sstrmdry.cpp"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/sstrstat.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/sstsetgl.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/sststat.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/ssttmul.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/sstvol.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/ssys.cpp"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/ssysinit.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/ssysserv.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/ssysveccsismutex.cpp"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/stagpat.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/stimemul.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/stimerem.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/stpparse.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/svecreal.cpp"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn/svol.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/extern/coda/cmn/coda.cpp"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/extern/coda/cmn/mtdecf.cpp"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/extern/coda/cmn/setmemcpy.cpp"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/gc/saramman.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/mix/scrsfl.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/mix/sfamplf.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/mix/sfbpffir8.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/mix/sfecho.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/mix/sfft24.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/mix/sfhpffir8.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/mix/sfilter.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/mix/sfir.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/mix/sfir8.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/mix/sflpf.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/mix/sflpffir8.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/mix/sfmixer.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/mix/sfreson.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/mix/sfsplit.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/mix/sfsrc.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/mix/sfxrevc.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/mix/sinit16.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/mix/sinitut.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/mix/sinitxa.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/mix/slinkmix.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/mix/smixc.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/mix/smixer.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/mix/smixfilt.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/mix/smixfram.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/mix/smixhip.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/mix/smixlowp.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/mix/smixptch.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/mix/smixtmul.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/mix/stretch.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/mix/supf.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/mix/suplf.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/mix/supmutf.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/mix/supmutlf.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/mix/supmutpf.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/mix/suppf.c"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/mix/supxaf.cpp"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/mix/supxalf.cpp"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/mix/supxapf.cpp"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/mix/sx87d16.c"
)
set_target_properties(snd_ea PROPERTIES LINKER_LANGUAGE C)
target_include_directories(snd_ea PRIVATE
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/cmn"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/source/library/mix"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/include"
    "${GAME_ROOT}/src/Speed/Indep/Libs/snd/9/extern"
    "${GAME_ROOT}/src/Speed/Indep/Libs/csis/dev/include"
    "${GAME_ROOT}/src/Speed/Indep/Libs/allocator/1.5.0"
    "${GAME_ROOT}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/platform"
)
target_compile_options(snd_ea PRIVATE -x c++ -O2 -Wno-everything -D__ANDROID__)
