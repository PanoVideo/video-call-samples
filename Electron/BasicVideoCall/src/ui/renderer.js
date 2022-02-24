
'use strict';

; (function (window, undefined) {
  const {ipcRenderer} = require('electron')
  const {viewMgr} = require('./media_views');
  const enginejs = require('../engine');

  let inputAppId = document.getElementById("text_appid")

  let btnJoinChannel = document.getElementById("btn_join_channel")
  let inputUserId = document.getElementById("text_user_id")
  let inputUserName = document.getElementById("text_user_name")
  btnJoinChannel.addEventListener("click", joinChannel)
  inputUserId.addEventListener("input", setUserName)

  function joinChannel() {
    let settings = {
      appId: inputAppId.value,
      token: document.getElementById('text_token').value,
      channelId: document.getElementById('text_channel_id').value,
      userId: inputUserId.value,
      userName: inputUserName.value,
      micphone: getMicphone(),
      speaker: getSpeaker(),
      camera: getCamera()
    };
    
    document.getElementById('div_media').style.display = 'block';
    document.getElementById('div_join').style.display = 'none';

    console.log('+++++ JS joinChannel');
    enginejs.joinChannel(settings);

    localStorage.setItem('pano_channel_settings', JSON.stringify(settings));
  }

  inputUserId.value = Math.trunc(Math.random() * 1000 + 17000);

  document.getElementById('div_media').style.display = 'none';

  let cbDevTools = document.getElementById("cb-dev-tools");
  cbDevTools.addEventListener("click", () => {
    ipcRenderer.send('toggle-devtools');
  });
  ipcRenderer.on('devtools-closed', (event, args) => {
    cbDevTools.checked = false;
  });
  cbDevTools.checked = true;

  window.addEventListener('load', (event) => {
    initMainPage();
    viewMgr.initViews();
  });

  function initMainPage() {
    let selectMicphone = document.getElementById('select_audio_micphone')
    selectMicphone.options.length=0
    selectMicphone.options.add(new Option('System Default', 'Default'))
    let mics = enginejs.rtcEngine.audioDeviceMgr().getRecordDeviceList();
    for (let v of mics) {
      selectMicphone.options.add(new Option(v.deviceName, v.deviceId))
    }

    let selectSpeaker = document.getElementById('select_audio_speaker')
    selectSpeaker.options.length=0
    selectSpeaker.options.add(new Option('System Default', 'Default'))
    let speakers = enginejs.rtcEngine.audioDeviceMgr().getPlayoutDeviceList();
    for (let v of speakers) {
      selectSpeaker.options.add(new Option(v.deviceName, v.deviceId))
    }

    let selectCamera = document.getElementById('select_video_camera')
    selectCamera.options.length=0;
    let cams = enginejs.rtcEngine.videoDeviceMgr().getCaptureDeviceList();
    for (let v of cams) {
      selectCamera.options.add(new Option(v.deviceName, v.deviceId))
    }

    let str_settings = localStorage.getItem('pano_channel_settings');
    if (str_settings) {
      let settings = JSON.parse(str_settings);
      if (settings) {
        inputAppId.value = settings.appId;
        document.getElementById('text_channel_id').value = settings.channelId;
        if(settings.userId) inputUserId.value = settings.userId;
        inputUserName.value = settings.userName;
        document.getElementById('text_token').value = settings.token;
      }
    }
  }

  function getMicphone() {
    let selectMicphone = document.getElementById('select_audio_micphone');
    let micphone = 'Default';
      
    if (selectMicphone.selectedIndex >= 0) {
      micphone = selectMicphone.options[selectMicphone.selectedIndex].value;
    }
    return micphone;
  }

  function getSpeaker() {
    let selectSpeaker = document.getElementById('select_audio_speaker');
    let speaker = 'Default';
    
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

  function setUserName(){
    inputUserName.value = "Electron_" + inputUserId.value;
  }

})(window);
