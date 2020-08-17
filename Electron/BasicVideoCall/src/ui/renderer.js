
'use strict';

; (function (window, undefined) {
  const {ipcRenderer} = require('electron')
  const {viewMgr} = require('./media_views');
  const enginejs = require('../engine');
  var joined = false;

  console.error("***** please enter your APPID and PANO token *****")
  var appId = '%% enter your APPID %%',
    token = '%% enter your token %%';

  let btnJoinChannel = document.getElementById("btn_join_channel")
  btnJoinChannel.addEventListener("click", joinChannel)
  function joinChannel() {
    if (!joined) {
      joined = true;
      let settings = {
        appId: appId,
        token: token,
        channelId: document.getElementById('text_channel_id').value,
        userId: document.getElementById('text_user_id').value,
        userName: document.getElementById('text_user_name').value,
        micphone: getMicphone(),
        speaker: getSpeaker(),
        camera: getCamera()
      };
      
      btnJoinChannel.innerText = 'Leave Channel';
      document.getElementById('div_media').style.display = 'block';
      document.getElementById('div_join').style.display = 'none';

      console.log('+++++ JS joinChannel');
      enginejs.joinChannel(settings);

      localStorage.setItem('pano_channel_settings', JSON.stringify(settings));
    } else {
      joined = false;
      enginejs.leaveChannel();
      btnJoinChannel.innerText = 'Join Channel';
    }
  }
  document.getElementById('text_user_id').value = Math.trunc(Math.random() * 1000 + 17000);

  document.getElementById('div_media').style.display = 'none';

  let cbDevTools = document.getElementById("cb-dev-tools");
  cbDevTools.addEventListener("click", () => {
    ipcRenderer.send('toggle-devtools');
  });
  ipcRenderer.on('devtools-closed', (event, args) => {
    cbDevTools.checked = false;
  });

  window.addEventListener('load', (event) => {
    initMainPage();
    viewMgr.initViews();
  });

  function initMainPage() {
    let selectMicphone = document.getElementById('select_audio_micphone')
    selectMicphone.options.length=0
    selectMicphone.options.add(new Option('System Default', 'Default'))
    let mics = enginejs.rtcEngine.audio.getRecordDeviceList();
    for (var v of mics) {
      selectMicphone.options.add(new Option(v.deviceName, v.deviceId))
    }

    let selectSpeaker = document.getElementById('select_audio_speaker')
    selectSpeaker.options.length=0
    selectSpeaker.options.add(new Option('System Default', 'Default'))
    let speakers = enginejs.rtcEngine.audio.getPlayoutDeviceList();
    for (var v of speakers) {
      selectSpeaker.options.add(new Option(v.deviceName, v.deviceId))
    }

    let selectCamera = document.getElementById('select_video_camera')
    selectCamera.options.length=0;
    let cams = enginejs.rtcEngine.video.getCaptureDeviceList();
    for (var v of cams) {
      selectCamera.options.add(new Option(v.deviceName, v.deviceId))
    }

    let str_settings = localStorage.getItem('pano_channel_settings');
    if (str_settings) {
      let settings = JSON.parse(str_settings);
      if (settings) {
        document.getElementById('text_channel_id').value = settings.channelId;
        document.getElementById('text_user_name').value = settings.userName;
      }
    }
  }

  function getMicphone() {
    let selectMicphone = document.getElementById('select_audio_micphone');
    var micphone = 'Default';
      
    if (selectMicphone.selectedIndex >= 0) {
      micphone = selectMicphone.options[selectMicphone.selectedIndex].value;
    }
    return micphone;
  }

  function getSpeaker() {
    let selectSpeaker = document.getElementById('select_audio_speaker');
    var speaker = 'Default';
    
    if (selectSpeaker.selectedIndex >= 0) {
      speaker = selectSpeaker.options[selectSpeaker.selectedIndex].value
    }
    return speaker;
  }

  function getCamera() {
    let selectCamera = document.getElementById('select_video_camera');
    if (selectCamera.selectedIndex < 0) {
      return null;
    }
    return selectCamera.options[selectCamera.selectedIndex].value;
  }

})(window);
