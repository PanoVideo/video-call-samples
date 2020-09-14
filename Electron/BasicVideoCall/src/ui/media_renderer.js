
'use strict';

; (function (window, undefined) {
  const {QResult} = require('@pano.video/panortc-electron-sdk/js/panodefs');
  const enginejs = require('../engine');

  var audioStarted = false,
    audioMuted = false,
    videoStarted = false,
    videoMuted = false,
    screenStarted = false,
    screenMuted = false;

  let btnLeaveChannel = document.getElementById("btn_leave_channel");
  btnLeaveChannel.addEventListener("click", () => {
    enginejs.leaveChannel();
    resetState();
    enableButtons(false);
    document.getElementById('div_media').style.display = 'none';
    document.getElementById('div_join').style.display = 'block';
  });

  let btnStartAudio = document.getElementById("btn_start_audio");
  btnStartAudio.addEventListener("click", () => {
    audioStarted = !audioStarted;
    if (audioStarted) {
      enginejs.startAudio();
    } else {
      enginejs.stopAudio();
    }
    btnStartAudio.innerText = audioStarted ? 'Stop Audio' : 'Start Audio';
    audioMuted = false;
    btnMuteAudio.innerText = 'Mute Audio';
  });
  let btnMuteAudio = document.getElementById("btn_mute_audio");
  btnMuteAudio.addEventListener("click", () => {
    if (audioStarted) {
      audioMuted = !audioMuted;
      if (audioMuted) {
        enginejs.muteAudio();
      } else {
        enginejs.unmuteAudio();
      }
      btnMuteAudio.innerText = audioMuted ? 'Unmute Audio' : 'Mute Audio';
    }
  });

  let btnStartVideo = document.getElementById("btn_start_video");
  btnStartVideo.addEventListener("click", () => {
    videoStarted = !videoStarted;
    if (videoStarted) {
      enginejs.startVideo();
    } else {
      enginejs.stopVideo();
    }
    btnStartVideo.innerText = videoStarted ? 'Stop Video' : 'Start Video';
    videoMuted = false;
    btnMuteVideo.innerText = 'Mute Video';
  });
  let btnMuteVideo = document.getElementById("btn_mute_video");
  btnMuteVideo.addEventListener("click", () => {
    if (videoStarted) {
      videoMuted = !videoMuted;
      if (videoMuted) {
        enginejs.muteVideo();
      } else {
        enginejs.unmuteVideo();
      }
      btnMuteVideo.innerText = videoMuted ? 'Unmute Video' : 'Mute Video';
    }
  });

  let btnStartScreen = document.getElementById("btn_start_screen");
  btnStartScreen.addEventListener("click", () => {
    screenStarted = !screenStarted;
    if (screenStarted) {
      enginejs.startScreen();
    } else {
      enginejs.stopScreen();
    }
    btnStartScreen.innerText = screenStarted ? 'Stop Screen' : 'Start Screen';
    screenMuted = false;
    btnMuteScreen.innerText = 'Mute Screen';
  });
  let btnMuteScreen = document.getElementById("btn_mute_screen");
  btnMuteScreen.addEventListener("click", () => {
    if (screenStarted) {
      screenMuted = !screenMuted;
      if (screenMuted) {
        enginejs.muteScreen();
      } else {
        enginejs.unmuteScreen();
      }
      btnMuteScreen.innerText = screenMuted ? 'Unmute Screen' : 'Mute Screen';
    }
  });

  window.addEventListener('load', (event) => {
    enableButtons(false);
  });

  enginejs.onEvent('channel-join-confirm', (result) => {
    if (result == QResult.OK) {
      enableButtons(true);
    }
  });
  enginejs.onEvent('channel-leave-indication', (result) => {
    enableButtons(false);
  });
  enginejs.onEvent('user-join-indication', (user) => {
    
  });
  enginejs.onEvent('user-leave-indication', (userId) => {
    
  });

  function resetState() {
    audioStarted = false;
    audioMuted = false;
    videoStarted = false;
    videoMuted = false;
    screenStarted = false;
    screenMuted = false;
    btnStartAudio.innerText = 'Start Audio';
    btnMuteAudio.innerText = 'Mute Audio';
    btnStartVideo.innerText = 'Start Video';
    btnMuteVideo.innerText = 'Mute Video';
    btnStartScreen.innerText = 'Start Screen';
    btnMuteScreen.innerText = 'Mute Screen';
  }
  function enableButtons(enabled) {
    btnStartAudio.disabled = !enabled;
    btnMuteAudio.disabled = !enabled;
    btnStartVideo.disabled = !enabled;
    btnMuteVideo.disabled = !enabled;
    btnStartScreen.disabled = !enabled;
    btnMuteScreen.disabled = !enabled;
  }

})(window);
