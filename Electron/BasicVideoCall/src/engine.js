
'use strict';

; (function (window, undefined) {
  const {
    RtcEngine,
    QResult, 
    ChannelMode,
    AudioAecType,
    kChannelServiceMedia,
    VideoProfileType,
    VideoScalingMode,
    ScreenSourceType
  } = require('@pano.video/panortc-electron-sdk');
  const {viewMgr} = require('./ui/media_views');
  const XMLHttpRequest = require("xmlhttprequest").XMLHttpRequest;
  const {EventEmitter} = require("events");
  let channelSettings = {};
  let userMgr;
  let emitter = new EventEmitter();
  let rtcEngine = new RtcEngine();

  console.log('+++++ JS SDK Version: ' + rtcEngine.getSdkVersion());

  rtcEngine.on('channelJoinConfirm', (result) => {
    console.log('+++++ JS onChannelJoinConfirm, result: ' + result);
    if (result != QResult.OK) {
      alert('Join channel failed, reason: ' + result);
    }
    emitter.emit('channel-join-confirm', result);
  })
  rtcEngine.on('channelLeaveIndication', (result) => {
    console.log('+++++ JS onChannelLeaveIndication, result: ' + result);
    alert('Channel left, reason: ' + result);
    viewMgr.freeViews();
    userMgr.removeAllUsers();
    emitter.emit('channel-leave-indication', result);
  })
  rtcEngine.on('channelCountDown', (remain) => {
    console.log('+++++ JS onChannelCountDown, remain: ' + remain)
  })
  rtcEngine.on('userJoinIndication', (userId, userName) => {
    console.log('+++++ JS onUserJoinIndication, userId: ' + userId + ', userName: ' + userName);
    userMgr.addUser(userId, userName);
    onUserJoin({userId: userId, userName: userName});
    emitter.emit('user-join-indication', {userId: userId, userName: userName});
  })
  rtcEngine.on('userLeaveIndication', (userId, reason) => {
    console.log('+++++ JS onUserLeaveIndication, userId: ' + userId + ', reason: ' + reason);
    viewMgr.stopRemoteUserViews(userId);
    userMgr.removeUser(userId);
    onUserLeave(userId);
    emitter.emit('user-leave-indication', userId);
  })
  rtcEngine.on('userAudioStart', (userId) => {
    console.log('+++++ JS onUserAudioStart, userId: ' + userId)
  })
  rtcEngine.on('userAudioStop', (userId) => {
    console.log('+++++ JS onUserAudioStop, userId: ' + userId)
  })
  rtcEngine.on('userAudioSubscribe', (userId, result) => {
    console.log('+++++ JS onUserAudioSubscribe, userId: ' + userId + ', result: ' + result)
  })
  rtcEngine.on('userVideoStart', (userId, profile) => {
    console.log('+++++ JS onUserVideoStart, userId: ' + userId + ', profile: ' + profile);
    userMgr.addVideoUser(userId, profile);
    let user = userMgr.getVideoUser(userId);
    subscribeUserVideo(user);
  })
  rtcEngine.on('userVideoStop', (userId) => {
    console.log('+++++ JS onUserVideoStop, userId: ' + userId);
    viewMgr.stopRemoteUserVideoView(userId);
    userMgr.removeVideoUser(userId);
  })
  rtcEngine.on('userVideoSubscribe', (userId, result) => {
    console.log('+++++ JS onUserVideoSubscribe, userId: ' + userId + ', result: ' + result)
  })
  rtcEngine.on('userAudioMute', (userId) => {
    console.log('+++++ JS onUserAudioMute, userId: ' + userId)
  })
  rtcEngine.on('userAudioUnmute', (userId) => {
    console.log('+++++ JS onUserAudioUnmute, userId: ' + userId)
  })
  rtcEngine.on('userVideoMute', (userId) => {
    console.log('+++++ JS onUserVideoMute, userId: ' + userId)
  })
  rtcEngine.on('userVideoUnmute', (userId) => {
    console.log('+++++ JS onUserVideoUnmute, userId: ' + userId)
  })
  rtcEngine.on('userScreenStart', (userId) => {
    console.log('+++++ JS onUserScreenStart, userId: ' + userId)
    userMgr.addScreenUser(userId);
    let user = userMgr.getScreenUser(userId);
    subscribeUserScreen(user);
  })
  rtcEngine.on('userScreenStop', (userId) => {
    console.log('+++++ JS onUserScreenStop, userId: ' + userId)
    viewMgr.stopRemoteUserScreenView(userId);
    userMgr.removeScreenUser(userId);
  })
  rtcEngine.on('userScreenSubscribe', (userId, result) => {
    console.log('+++++ JS onUserScreenSubscribe, userId: ' + userId + ', result: ' + result)
  })
  rtcEngine.on('userScreenMute', (userId) => {
    console.log('+++++ JS onUserScreenMute, userId: ' + userId)
  })
  rtcEngine.on('userScreenUnmute', (userId) => {
    console.log('+++++ JS onUserScreenUnmute, userId: ' + userId)
  })
  rtcEngine.on('whiteboardAvailable', () => {
    console.log('+++++ JS onWhiteboardAvailable')
  })
  rtcEngine.on('whiteboardUnavailable', () => {
    console.log('+++++ JS onWhiteboardUnavailable')
  })
  rtcEngine.on('whiteboardStart', () => {
    console.log('+++++ JS onWhiteboardStart')
  })
  rtcEngine.on('whiteboardStop', () => {
    console.log('+++++ JS onWhiteboardStop')
  })
  rtcEngine.on('firstAudioDataReceived', (userId) => {
    console.log('+++++ JS onFirstAudioDataReceived, userId: ' + userId)
  })
  rtcEngine.on('firstVideoDataReceived', (userId) => {
    console.log('+++++ JS onFirstVideoDataReceived, userId: ' + userId)
  })
  rtcEngine.on('firstScreenDataReceived', (userId) => {
    console.log('+++++ JS onFirstScreenDataReceived, userId: ' + userId)
  })
  rtcEngine.on('audioDeviceStateChanged', (deviceID, deviceType, deviceState) => {
    console.log('+++++ JS onAudioDeviceStateChanged, id: ' + deviceID + ', type: ' + deviceType + ', state: ' + deviceState)
  })
  rtcEngine.on('audioDefaultDeviceChanged', (deviceID, deviceType) => {
    console.log('+++++ JS onAudioDefaultDeviceChanged, id: ' + deviceID + ', type: ' + deviceType)
  })
  rtcEngine.on('videoDeviceStateChanged', (deviceID, deviceType, deviceState) => {
    console.log('+++++ JS onVideoDeviceStateChanged, id: ' + deviceID + ', type: ' + deviceType + ', state: ' + deviceState)
  })
  rtcEngine.on('videoCaptureStateChanged', (deviceID, captureState) => {
    console.log('+++++ JS onVideoCaptureStateChanged, id: ' + deviceID + ', state: ' + captureState)
  })
  rtcEngine.on('channelFailover', (state) => {
    console.log('+++++ JS onChannelFailover, state: ' + state)
  })
  rtcEngine.on('activeSpeakerListUpdated', (userList) => {
    console.log('+++++ JS onActiveSpeakerListUpdated, userList: ' + userList)
  })
  rtcEngine.on('audioMixingStateChanged', (taskId, state) => {
    console.log('+++++ JS onAudioMixingStateChanged, taskId: ' + taskId + ', state: ' + state)
  })
  rtcEngine.on('videoSnapshotCompleted', (succeed, userId, fileName) => {
    console.log('+++++ JS onVideoSnapshotCompleted, userId: ' + userId + ', succeed: ' + succeed)
  })
  rtcEngine.on('networkQuality', (userId, quality) => {
    //console.log('+++++ JS onNetworkQuality, userId: ' + userId + ', quality: ' + quality)
  })


  function unsubscribeAllUserVideo() {
    let users = userMgr.getVideoUserList();
    for (let user of users) {
      if (user.subscribed) {
        rtcEngine.unsubscribeVideo(user.userId);
        viewMgr.stopRemoteUserVideoView(user.userId);
        user.subscribed = false;
      }
    }
  }
  function subscribeAllUserVideo() {
    let users = userMgr.getVideoUserList();
    for (let user of users) {
      if (!user.subscribed) {
        if (!subscribeUserVideo(user)) {
          break;
        }
      }
    }
  }
  function subscribeUserVideo(user) {
    let vi = viewMgr.allocRemoteUserVideoView(user.userId);
    if (vi != null) {
      let profile = vi.isMain ? user.profile : VideoProfileType.Low;
      let ret = rtcEngine.subscribeVideo(user.userId, vi.view, 
        {profile: profile, scaling: VideoScalingMode.Fit, mirror: false});
      if (ret == QResult.OK) {
        viewMgr.showRemoteUserVideoView(user.userId);
        user.subscribed = true;
      }
      return true;
    }
    return false;
  }
  function unsubscribeUserVideo(userId) {
    rtcEngine.unsubscribeVideo(userId);
    viewMgr.stopRemoteUserVideoView(userId);
  }
  function subscribeUserScreen(user) {
    let mvi = viewMgr.getMainViewInfo();
    if (mvi != null) {
      if (!mvi.isFree) {
        if (mvi.isScreen) {
          rtcEngine.unsubscribeScreen(mvi.userId);
        } else {
          rtcEngine.unsubscribeVideo(mvi.userId);
        }
      }
      mvi.isFree = false;
      mvi.isScreen = true;
      mvi.userId = user.userId;
      let ret = rtcEngine.subscribeScreen(user.userId, mvi.view);
      if (ret == QResult.OK) {
        viewMgr.showRemoteUserVideoView(user.userId);
        user.subscribed = true;
      }
      return true;
    }
    return false;
  }

  function joinChannel(settings) {
    userMgr = new UserManager();
    let appId = settings.appId;
    let token = settings.token;
    let channelId = settings.channelId;
    let userId = settings.userId;
    let userName = settings.userName;
    channelSettings = settings;
    console.log('+++++ JS joinChannel, channelId: ' + channelId + ', userId: ' + userId);
    console.log('+++++ JS joinChannel, mic: ' + settings.micphone + ', camera: ' + settings.camera);

    rtcEngine.initialize(appId, {
      videoHwAccel: false, 
      audioAecType: AudioAecType.Default, 
      audioScenario: 0
    });

    doJoinChannel(token, channelId, userId, userName);
    addLocalUser(userId);
  }

  function leaveChannel() {
    rtcEngine.leaveChannel();
    viewMgr.freeViews();
    userMgr.removeAllUsers();
    clearUserList();
  }

  function doJoinChannel(token, channelId, userId, userName) {
    let serviceFlags = kChannelServiceMedia;
    let ret = rtcEngine.joinChannel(token, channelId, userId, {
      channelMode: ChannelMode.Mode_Meeting,
      serviceFlags: serviceFlags,
      subscribeAudioAll: true, // subscribe user audio automatically
      userName: userName
    });
    if (ret != QResult.OK) {
      console.warn(`+++++ JS doJoinChannel, failed to join channel, result: ${ret}`);
    }
  }

  function startAudio() {
    let micphone = channelSettings.micphone;
    let speaker = channelSettings.speaker;
    if (micphone != 'Default') {
      rtcEngine.audioDeviceMgr().setRecordDevice(micphone)
    } else {
      rtcEngine.audioDeviceMgr().setDefaultRecordDevice();
    }
    if (speaker != 'Default') {
      rtcEngine.audioDeviceMgr().setPlayoutDevice(speaker)
    } else {
      rtcEngine.audioDeviceMgr().setDefaultPlayoutDevice();
    }
    return rtcEngine.startAudio();
  }
  function stopAudio() {
    return rtcEngine.stopAudio();
  }
  function muteAudio() {
    return rtcEngine.muteAudio();
  }
  function unmuteAudio() {
    return rtcEngine.unmuteAudio();
  }
  function startVideo() {
    let camera = channelSettings.camera;
    if (!camera) {
      return;
    }
    rtcEngine.videoDeviceMgr().setDevice(camera);
    let view = viewMgr.getLocalView();
    if (view != null) {
      let ret = rtcEngine.startVideo(view, {
        profile: VideoProfileType.HD720P, 
        scaling: VideoScalingMode.Fit, 
        mirror: true
      })
      if (ret == QResult.OK) {
        viewMgr.showLocalView();
      }
    }
  }
  function stopVideo() {
    rtcEngine.stopVideo();
    viewMgr.stopLocalView();
  }
  function muteVideo() {
    return rtcEngine.muteVideo();
  }
  function unmuteVideo() {
    return rtcEngine.unmuteVideo();
  }
  function startScreen() {
    let displayList = rtcEngine.screenSourceMgr().getDisplayList();
    rtcEngine.screenSourceMgr().setScreenSource(ScreenSourceType.Display, displayList[0].ssid);
    rtcEngine.startScreen();
  }
  function stopScreen() {
    return rtcEngine.stopScreen()
  }
  function muteScreen() {
    return rtcEngine.muteScreen();
  }
  function unmuteScreen() {
    return rtcEngine.unmuteScreen();
  }


  function onUserJoin(user) {
    let row = document.getElementById('tb-user-list').insertRow();
    let cell = row.insertCell();
    cell.innerHTML = user.userId;
    cell = row.insertCell();
    cell.innerHTML = user.userName;
  }
  function onUserLeave(userId) {
    let tb = document.getElementById('tb-user-list');
    for (let i=0; i<tb.rows.length; i++) {
      if (userId === tb.rows[i].cells[0].innerText) {
        tb.deleteRow(i);
        break;
      }
    }
  }
  function addLocalUser(userId) {
    let row = document.getElementById('tb-user-list').insertRow();
    let cell = row.insertCell();
    cell.innerHTML = '<b>' + userId + '</b>';
    cell = row.insertCell();
    cell.innerHTML = '<b>Me</b>';
  }
  function clearUserList() {
    let tb = document.getElementById('tb-user-list');
    while(tb.hasChildNodes()) {
      tb.removeChild(tb.firstChild);
    }
  }



  class UserManager {
    constructor() {
      this.userList = new Map();
      this.videoUserList = new Map();
      this.screenUserList = new Map();
    }

    addUser(userId, userName) {
      this.userList.set(userId, {userId: userId, userName: userName});
    }
    removeUser(userId) {
      this.userList.delete(userId);
      this.videoUserList.delete(userId);
      this.screenUserList.delete(userId);
    }
    removeAllUsers() {
      this.userList = new Map();
      this.videoUserList = new Map();
      this.screenUserList = new Map();
    }
    addVideoUser(userId, profile) {
      this.videoUserList.set(userId, {userId: userId, profile: profile, subscribed: false});
    }
    removeVideoUser(userId) {
      this.videoUserList.delete(userId);
    }
    getVideoUser(userId) {
      return this.videoUserList.get(userId);
    }
    addScreenUser(userId) {
      this.screenUserList.set(userId, {userId: userId, subscribed: false});
    }
    removeScreenUser(userId) {
      this.screenUserList.delete(userId);
    }
    getScreenUser(userId) {
      return this.screenUserList.get(userId);
    }
    getUserList() {
      let users = [];
      for (let user of this.userList.values()) {
        users[users.length] = user;
      }
      return users;
    }
    getVideoUserList() {
      let users = [];
      for (let user of this.videoUserList.values()) {
        users[users.length] = user;
      }
      return users;
    }
    getScreenUserList() {
      let users = [];
      for (let user of this.screenUserList.values()) {
        users[users.length] = user;
      }
      return users;
    }
  }

  function onEvent(event, args) {
    emitter.on(event, args);
  }



  function uuidv4() {
    return 'xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx'.replace(/[xy]/g, function(c) {
      let r = Math.random() * 16 | 0, v = c == 'x' ? r : (r & 0x3 | 0x8);
      return v.toString(16);
    });
  }

  module.exports = 
  {
    rtcEngine: rtcEngine,
    joinChannel: joinChannel,
    leaveChannel: leaveChannel,
    startAudio: startAudio,
    stopAudio: stopAudio,
    muteAudio: muteAudio,
    unmuteAudio: unmuteAudio,
    startVideo: startVideo,
    stopVideo: stopVideo,
    muteVideo: muteVideo,
    unmuteVideo: unmuteVideo,
    startScreen: startScreen,
    stopScreen: stopScreen,
    muteScreen: muteScreen,
    unmuteScreen: unmuteScreen,
    onEvent: onEvent,
  };

})(window);
