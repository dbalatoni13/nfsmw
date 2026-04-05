#include "Speed/Indep/Src/EAXSound/SndCamera.hpp"

Camera *SndCamera::GetCam(int nPlayer) {
    if (m_pCams[nPlayer]) {
        return m_pCams[nPlayer];
    }

    return m_pCams[0];
}
