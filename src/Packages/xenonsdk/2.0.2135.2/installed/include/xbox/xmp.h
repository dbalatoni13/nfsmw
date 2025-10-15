/**************************************************************************
 *
 *  Copyright (C) 2003 - 2005 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       xmp.h
 *  Content:    Xenon Music Player API.
 *
 **************************************************************************/

#ifndef __XMP_INCLUDED__
#define __XMP_INCLUDED__

#include <xtl.h>
#include <xam.h>
#include <xaudio.h>

#ifdef __cplusplus
extern "C" {
#endif

//
// Create title playlist flags
//

#define XMP_CREATETITLEPLAYLISTFLAG_NONE        (0x0000000)                  

//
// Pass this flag to XMPCreateTitlePlaylist() to make title playlists show up
// in the HUD and be accessible to the user.
// If this flag is set the playlist can never be deleted.
//

#define XMP_CREATETITLEPLAYLISTFLAG_PERMANENT   (0x0000001)                  
   
//
// Enumeration flags
//

#define XMP_CREATEENUMERATORFLAG_NONE        	(0x0000000)                  
#define XMP_CREATEENUMERATORFLAG_FOREGROUNDPRI  (0x0000001)  

//
// XMP playback flags
//

#define XMP_PLAYBACKFLAG_NONE                   (0x0000000)     

//
// This flag will cause playback to goto into a paused state just prior
// to starting playback.
//
#define XMP_PLAYBACKFLAG_AUTOPAUSE              (0x0000001)                  
    
//
// Character lengths of metadata buffers.
//

#define XMP_MAX_METADATASTRING                  (40)

//
// Handle to title playlists and songs.
//

typedef LPVOID XMP_HANDLE, *LPXMP_HANDLE;                         

//
// Metadata for a song
//

#define XMP_SONGRESERVEDFIELDSIZE                (572)
typedef struct XMP_SONGINFO
{
    XMP_HANDLE hSong;                                       // Song handle.
    BYTE  abReserved[XMP_SONGRESERVEDFIELDSIZE];            // Reserved field.
    WCHAR wszTitle[XMP_MAX_METADATASTRING];                 // Song title.
    WCHAR wszArtist[XMP_MAX_METADATASTRING];                // Song artist.
    WCHAR wszAlbum[XMP_MAX_METADATASTRING];                 // Song album.
    WCHAR wszAlbumArtist[XMP_MAX_METADATASTRING];           // Song album artist.
    WCHAR wszGenre[XMP_MAX_METADATASTRING];                 // Genre.
    DWORD dwTrackNumber;                                    // Track number in the album (0 indicates an invalid number).
    DWORD dwDuration;                                       // Duration in milliseconds.
    DWORD dwReserved1;                                      // Reserved field. 
    DWORD dwReserved2;                                      // Reserved field.
} XMP_SONGINFO, *LPXMP_SONGINFO;

typedef const LPXMP_SONGINFO LPCXMP_SONGINFO;

//
// Metadata for a user playlist.
//

#define XMP_MAX_USERPLAYLISTID                      (572)
typedef BYTE XMP_USERPLAYLISTID[XMP_MAX_USERPLAYLISTID], *LPXMP_USERPLAYLISTID;

#define XMP_USERPLAYLISTRESERVEDFIELDSIZE           (168)

typedef struct XMP_USERPLAYLISTINFO
{
    XMP_USERPLAYLISTID Id;                                  // Id.
    WCHAR wszTitle[XMP_MAX_METADATASTRING];                 // Container title.
    BYTE Reserved[XMP_USERPLAYLISTRESERVEDFIELDSIZE];       // Reserved field.
} XMP_USERPLAYLISTINFO, *LPXMP_USERPLAYLISTINFO;            

typedef const LPXMP_USERPLAYLISTINFO LPCXMP_USERPLAYLISTINFO;

//
// Status returned by calling XMPGetStatus().
//

typedef enum XMP_STATE
{
    XMP_STATE_IDLE = 0,                                     // XMP is idle and not playing anything.
    XMP_STATE_PLAYING,                                      // Playback of a song is occuring.
    XMP_STATE_PAUSED,                                       // Playback of a song has been paused.
} XMP_STATE, *LPXMP_STATE;

typedef const LPXMP_STATE LPCXMP_STATE;

//
// Playback modes 
//

typedef enum XMP_PLAYBACKMODE
{
    XMP_PLAYBACKMODE_INORDER = 0,
    XMP_PLAYBACKMODE_SHUFFLE,
} XMP_PLAYBACKMODE, *LPXMP_PLAYBACKMODE;

typedef const LPXMP_PLAYBACKMODE LPCXMP_PLAYBACKMODE;

//
// Repeat modes.
//

typedef enum XMP_REPEATMODE
{
    XMP_REPEATMODE_PLAYLIST = 0,
    XMP_REPEATMODE_NOREPEAT,
} XMP_REPEATMODE, *LPXMP_REPEATMODE;

typedef const LPXMP_REPEATMODE LPCXMP_REPEATMODE;

//
// Types of songs that can be added to a playlist.
//

typedef enum XMP_SONGFORMAT
{
    XMP_SONGFORMAT_WMA = 0,
    XMP_SONGFORMAT_MP3,
} XMP_SONGFORMAT, *LPXMP_SONGFORMAT;

typedef const LPXMP_SONGFORMAT LPCXMP_SONGFORMAT;

//
// Song to be added to a title playlist.
//

typedef struct XMP_SONGDESCRIPTOR
{
    LPCWSTR        pwszFilePath;                            // Path to the song being added to the playlist.
    LPCWSTR        pwszTitle;                               // Song title.
    LPCWSTR        pwszArtist;                              // Song artist.
    LPCWSTR        pwszAlbum;                               // Song album.
    LPCWSTR        pwszAlbumArtist;                         // Song album artist.
    LPCWSTR        pwszGenre;                               // Genre.
    DWORD          dwTrackNumber;                           // Track number in the album.
    DWORD          dwDuration;                              // Duration in milliseconds.
    XMP_SONGFORMAT eSongFormat;                             // Format of the song.
} XMP_SONGDESCRIPTOR, *LPXMP_SONGDESCRIPTOR;            

typedef const LPXMP_SONGDESCRIPTOR LPCXMP_SONGDESCRIPTOR;

//
// Creates a title playlist to playback songs. 
//

DWORD
WINAPI
XMPCreateTitlePlaylist
(
    IN LPCXMP_SONGDESCRIPTOR paSongs,                       // Array of songs to be placed into the playlist.
    IN DWORD dwNumSongs,                                    // The length of the song array.                                  
    IN DWORD dwFlags,                                       // Flags.
    IN LPCWSTR pwszPlaylistName,                            // Friendly name for display in the UI.     
                                                            // (Cannot be NULL if the flag XMP_CREATETITLEPLAYLISTFLAG_PERMANENT is used).
    OUT LPXMP_HANDLE pahSongs,                              // Array of handles to the added songs.
                                                            // (If non NULL this array must be at least dwNumSongs in length).
    OUT LPXMP_HANDLE phPlaylist                             // Handle to a playlist.
);

//
// Deletes a title playlist. All song ids from the playlist become invalid.
// Any attempt to delete a permanent title playlist will cause return ERROR_ACCESS_DENIED.
//

DWORD
WINAPI
XMPDeleteTitlePlaylist
(
    IN XMP_HANDLE hTitlePlaylist                            // Handle to a playlist.
);

//
// Returns information about the currently active song.
//

DWORD
WINAPI
XMPGetCurrentSong
(
    OUT LPXMP_SONGINFO pSongInfo                            // Currently playing song.
);

//
// Tries to start playback of a title created playlist.
//

VOID
WINAPI
XMPPlayTitlePlaylist
(
    IN XMP_HANDLE hPlaylist,                                // Title created playlist to play.
    IN XMP_HANDLE hSong                                     // Song to begin playback from. 
                                                            // (NULL indicates start from the first song).
);

//
// Tries to start playback of a user owned playlist.
//

VOID
WINAPI
XMPPlayUserPlaylist
(
    IN LPXMP_USERPLAYLISTINFO pUserPlaylist                  // Users playlist to playback.
);

//
// Tries to resume previously paused playback. 
//

VOID
WINAPI
XMPContinue();

//
// Tries to stop playback. 
//

VOID
WINAPI
XMPStop();

//
// Tries to pause playback. 
//

VOID
WINAPI
XMPPause();

//
// Tries to start playback of the next song. 
//

VOID
WINAPI
XMPNext();

//
// Tries to start playback of the previous song. 
//

VOID
WINAPI
XMPPrevious();

//
// Tries to sets the playback behavior (repeat mode, playback mode, ..) 
// of XMP. 
//

DWORD
WINAPI
XMPSetPlaybackBehavior
(
    IN XMP_PLAYBACKMODE ePlaybackMode,                      // The playback mode to use.
    IN XMP_REPEATMODE eRepeatMode,                          // The repeat mode to use.
    IN DWORD dwFlags                                        // Other playback flags.
);

//
// Gets the current state of XMP. 
//

DWORD
WINAPI
XMPGetStatus
(
    OUT LPXMP_STATE peState                                 // The current state of XMP
);

//
// Gets the volume that XMP is using in the range 0-1 
//

DWORD
WINAPI
XMPGetVolume
(
    OUT FLOAT* pfVolume                                     // The volume XMP is using.
);

//
// Sets the volume that XMP uses. 
//

DWORD
WINAPI
XMPSetVolume
(
    IN FLOAT fVolume                                        // The new volume
);

//
// Gets the playback behavior (repeat mode, playback mode, ..) 
// of XMP. 
//

DWORD
WINAPI
XMPGetPlaybackBehavior
(
    OUT LPXMP_PLAYBACKMODE pePlaybackMode,                  // The playback mode.
    OUT LPXMP_REPEATMODE peRepeatMode,                      // The repeat mode.
    OUT LPDWORD pdwFlags                                    // Other playback flags.
);

//
// Creates a object used to enumerate all the user playlists that are 
// currently available. Subsequent calls to XEnumerate() using the 
// created enumerator will retrieve the playlists.
//
// Memory for the enumerator is created out of the caller's budget. 
//

DWORD
WINAPI
XMPCreateUserPlaylistEnumerator
(
    IN DWORD dwFlags,                                       // Enumeration flags.
    IN DWORD cItems,                                        // Number of items we want to enumerate.
    OUT DWORD* pcbBuffer,                                   // Size of the buffer required to get the playlists.
    OUT HANDLE* ph                                          // Handle to the enumerator object.
);

//
// Determines if the title has playback control. 
//

DWORD
WINAPI
XMPTitleHasPlaybackControl
(
    OUT LPBOOL pfTitleHasControl                             // Is the title in control of playback.
);

//
// Disables the users music, allowing the game to control XMP and 
// preventing the user from re-gaining control, the Mini Player will 
// be disabled, until XMPRestorebackgroundMusic.
// Even if the user wasn't controlling XMP at the time of this call 
// the Mini Player is still disabled to prevent them from taking control.
//

DWORD
WINAPI
XMPOverrideBackgroundMusic();

//
// Restores the previous controller prior to the call of 
// XMPOverrideBackgroundMusic and enables the Mini Player.
//

DWORD
WINAPI
XMPRestoreBackgroundMusic();

//
// Retrieves output samples from the media player so they can be monitored
// or rendered by the title.  The callback supplied to XMPCaptureOutput
// will receive one audio frame of stereo samples.  The capture buffer will 
// contain one frame of stereo floating-point audio samples at 48 kHz.
//
// The fTitleRendering parameter passed to XMPCaptureOutput sets 
// whether the title wishes to render the music decoded by XMP.  If 
// fTitleRendering is TRUE, the title must render it using XAudio.  As long 
// as XMP is playing a title playlist, it will not render any of the audio 
// it's decoding.  If the user chooses to override the title music, XMP will 
// automatically start rendering music again.  At that point, the capture 
// callback will start receiving FALSE as its fTitleRendering parameter and
// the title should immediately stop rendering data, although monitoring it
// is still allowed.  If the title is rendering using an XAudio voice tagged as 
// XAUDIOVOICECATEGORY_BGMUSIC, the voice will automatically be muted and
// unmuted when the user takes or relenquishes control of the music playback.
//

#define XMP_CAPTURE_CHANNEL_COUNT   2
#define XMP_CAPTURE_SAMPLE_COUNT    (XMP_CAPTURE_CHANNEL_COUNT * XAUDIOFRAMESIZE_NATIVE)

typedef XAUDIOSAMPLE XMP_CAPTURE_BUFFER[XMP_CAPTURE_SAMPLE_COUNT], *LPXMP_CAPTURE_BUFFER;
typedef const XMP_CAPTURE_BUFFER *LPCXMP_CAPTURE_BUFFER;

typedef void (CALLBACK *LPFNXMP_CAPTURE_CALLBACK)
(
    IN LPCXMP_CAPTURE_BUFFER pSampleBuffer,
    IN LPVOID pContext,
    IN BOOL fTitleRendering
);

DWORD
WINAPI
XMPCaptureOutput
(
    IN LPFNXMP_CAPTURE_CALLBACK pfnCallback,
    IN LPVOID pContext,
    IN BOOL fTitleRendering
);

#ifdef __cplusplus
}
#endif

#endif // __XMP_INCLUDED__
