import PanoRtc from '@pano.video/panortc'
import { find } from 'lodash-es';
let PanoDemo = {
  users: []
};

// UI
const appId = document.getElementById('appID').value || 'temp';
const countdownDic = document.getElementById('countdown');

const selfVideoContainer = document.getElementById('selfVideoArea');
const previewVideoContainer = document.getElementById('previewVideoArea');
const activeVideoContainer = document.getElementById('activeVideoArea');
activeVideoContainer.ondblclick = switchFullScreen;
let fullscreen = false;

const button_joinChannel = document.getElementById('joinChannel');
const button_leaveChannel = document.getElementById('leaveChannel');
const textArea_roster = document.getElementById('meeting_roster');

const button_audio = document.getElementById('audioButton');
const button_video = document.getElementById('videoButton');
const button_share = document.getElementById('shareButton');

const video_user_list = document.getElementById('videoUsers');
const button_subscribe_video = document.getElementById('subscribeVideo');
button_subscribe_video.onclick = subscribeVideo;
const button_unsubscribe_video = document.getElementById('unSubscribeVideo');
button_unsubscribe_video.onclick = unSubscribeVideo;
const sub_video_quality = document.getElementById('subQuality');

const button_mute_mic = document.getElementById('muteMic');
const button_mute_screen = document.getElementById('muteScreen');
const button_get_mic = document.getElementById('getMics');
const button_get_speaker = document.getElementById('getSpeakers');
const button_get_cam = document.getElementById('getCams');
const button_set_cam = document.getElementById('setCam');

const share_user_list = document.getElementById('screenUsers');
const button_subscribe_share = document.getElementById('subscribeScreen');
button_subscribe_share.onclick = subscribeShare;
const button_unsubscribe_share = document.getElementById('unsubscribeScreen');
button_unsubscribe_share.onclick = unSubscribeShare;

const select_mic = document.getElementById('mic_sel');
const select_speaker = document.getElementById('speaker_sel');
const select_cam = document.getElementById('cam_sel');
const button_get_preview = document.getElementById('getPreview');
const button_snapshot_video = document.getElementById('snapshotVideo');
button_snapshot_video.onclick = pano_snapshotMyself;
button_get_mic.onclick = pano_getMics;
button_get_speaker.onclick = pano_getSpeakers;
button_get_cam.onclick = pano_getCams;
button_set_cam.onclick = pano_setCam;
button_get_preview.onclick = pano_getPreview;
button_snapshot_video.onclick = pano_snapshotMyself;


window.PanoDemo = PanoDemo;


function init_UI () {
  button_mute_mic.disabled = true;
  button_mute_screen.disabled = true;
  button_audio.disabled = true;
  button_video.disabled = true;
  button_share.disabled = true;
  button_subscribe_video.disabled = true;
  button_unsubscribe_video.disabled = true;

  button_joinChannel.onclick = joinChannel;
  init_Btn_Stat();
};

function init_Btn_Stat(){
  button_mute_mic.innerHTML = 'Mute Mic';
  button_mute_mic.onclick = pano_muteMic;
  button_audio.innerHTML = 'Start Audio';
  button_audio.onclick = startAudio;
  button_video.innerHTML = 'Start Video';
  button_video.onclick = startVideo;
  button_share.innerHTML = 'Start Screen';
  button_share.onclick = startShare;
  button_mute_screen.innerHTML = 'Mute Screen';
  button_mute_screen.onclick = pano_muteScreen;
}

/*****************************************************************************************************************
 *                                         Global UI Functions                                                   *
 *****************************************************************************************************************
 */
function joinChannel() {
  button_joinChannel.disabled = true;
  button_joinChannel.style.color = 'black';

  let rtcServer = document.getElementById('rtcServer').value
  let rtcEngine = new PanoRtc.RtcEngine({
    appId,
    rtcServer: rtcServer || 'https://aisle.pano.video'
  });
  console.info('Pano SDK Version: ' + rtcEngine.getSdkVersion());
  // For easily debug
  window.rtcEngine = rtcEngine;
  const eventTextarea = document.getElementById('events');
  rtcEngine.on = new Proxy(rtcEngine.on, {
    apply(target, object, args) {
      Reflect.apply(target, object, [args[0], params => {
        eventTextarea.value += `${JSON.stringify(params)}\r\n \r\n`;
        eventTextarea.scrollTop = eventTextarea.scrollHeight;
        Reflect.apply(args[1], object, [params]);
      }]);
    }
  });

  /*****************************************************************************************************************
   *                                         Events Handlers                                                       *
   *****************************************************************************************************************
  */

  rtcEngine.on(PanoRtc.RtcEngine.Events.joinChannelConfirm, data => {
    if (data.result !== 'success') {
      button_leaveChannel.disabled = true;
      button_leaveChannel.style.color = 'black';
      button_joinChannel.disabled = false;
      button_joinChannel.style.color = 'green';
      window.alert(`join channel failed because: ${data.message}`);
      return;
    }
    console.log('join channel success!');
    button_leaveChannel.disabled = false;
    button_leaveChannel.style.color = 'red';
    button_leaveChannel.onclick = () => leaveChannel();
    button_mute_mic.disabled = false;
    button_mute_screen.disabled = false;
    button_audio.disabled = false;
    button_video.disabled = false;
    button_share.disabled = false;
    button_subscribe_video.disabled = false;
    button_unsubscribe_video.disabled = false;
  });

  rtcEngine.on(PanoRtc.RtcEngine.Events.userListChange, result => {
    console.log('demo app: rosterChange', result);
    PanoDemo.users = result.users.map(user => {
      const oldUser = find(PanoDemo.users, { userId: user.userId }) || {};
      return Object.assign(oldUser, user);
    });
    updateRoster();
  });

  rtcEngine.on(PanoRtc.RtcEngine.Events.userLeave, (data) => {
    console.log('demo app: userleave,', data);
    unSubscribeVideo(null, data.userId);
  });
  rtcEngine.on(PanoRtc.RtcEngine.Events.userJoin, (data) => {
    console.log('demo app: userjoin,', data);
  });
  rtcEngine.on(PanoRtc.RtcEngine.Events.failedToSubscribeVideo, (data) =>
    console.log('demo app: failed_to_subscribe_video,', data)
  );
  rtcEngine.on(PanoRtc.RtcEngine.Events.userAudioMute, (data) =>
    console.log('demo app: userAudioMute,', data)
  );
  rtcEngine.on(PanoRtc.RtcEngine.Events.userAudioUnmute, (data) =>
    console.log('demo app: userAudioUnmute,', data)
  );
  rtcEngine.on(PanoRtc.RtcEngine.Events.whiteboardAvailable, (data) =>
    console.log('demo app: whiteboardAvailable', data)
  );
  rtcEngine.on(PanoRtc.RtcEngine.Events.whiteboardUnavailable, (data) =>
    console.log('demo app: whiteboardUnavailable', data)
  );
  rtcEngine.on(PanoRtc.RtcEngine.Events.whiteboardStart, (data) =>
    console.log('demo app: whiteboardStart', data)
  );
  rtcEngine.on(PanoRtc.RtcEngine.Events.whiteboardStop, (data) =>
    console.log('demo app: whiteboardStop', data)
  );
  rtcEngine.on(PanoRtc.RtcEngine.Events.firstAudioDataReceived, (data) =>
    console.log('demo app: firstAudioDataReceived', data)
  );
  rtcEngine.on(PanoRtc.RtcEngine.Events.channelFailover, (data) =>
    console.error('demo app: channelFailover', data)
  );
  rtcEngine.on(PanoRtc.RtcEngine.Events.audioDeviceChange, (data) =>
    console.log('demo app: audioDeviceChange', data)
  );
  rtcEngine.on(PanoRtc.RtcEngine.Events.videoDeviceChange, (data) =>
    console.log('demo app: videoDeviceChange', data)
  );
  rtcEngine.on(PanoRtc.RtcEngine.Events.userExpelled, (data) =>
    console.log('demo app: userExpelled', data)
  );
  rtcEngine.on(PanoRtc.RtcEngine.Events.userAudioStart, (data) =>
    console.log('demo app: userAudioStart', data)
  );
  rtcEngine.on(PanoRtc.RtcEngine.Events.channelCountDown, (data) => {
    console.log('demo app: channelCountDown', data);
    PanoDemo.remainsec = data.remainsec;
    countdownDic.style.display = 'block';
    countdownDic.innerHTML = `剩余时间: ${PanoDemo.remainsec}s`;
    const interval = setInterval(() => {
      if (PanoDemo.remainsec > 0) {
        countdownDic.innerHTML = `剩余时间: ${--PanoDemo.remainsec}s`;
      } else {
        clearInterval(interval);
      }
    }, 1000);
  });
  rtcEngine.on(PanoRtc.RtcEngine.Events.leaveChannelIndication, (data) => {
    console.log('demo app: leaveChannelIndication', data);
    leaveChannel(true);
  });
  rtcEngine.on(PanoRtc.RtcEngine.Events.enumerateDeviceTimeout, (data) => {
    console.log('demo app: enumerateDeviceTimeout', data);
  });
  // rtcEngine.on(PanoRtc.RtcEngine.Events.activeSpeakerListUpdate, data => console.log('demo app: activeSpeakerListUpdate', data))

  rtcEngine.on(PanoRtc.RtcEngine.Events.userVideoReceived, (data) => {
    console.log('demo app: receive remote video,', data);
    if (data.data.videoTag) {
      if (data.data.videoTag.id === `video${data.data.userId}`) {
        console.log(`get remote video again video${data.data.userId}`);
        return;
      }
      data.data.videoTag.id = `video${data.data.userId}`;
      const box = document.createElement('div');
      const title = document.createElement('div');
      title.innerHTML = 'user:' + data.data.userName + '(' + data.data.userId + ')';
      title.setAttribute('style', 'width:320px;font-size:8px;opacity:0.6;top:-22px;left:0px; background:#00BCD4;position:relative');
      box.setAttribute('style', 'width:320px;height:180px;display:inline-block');
      box.setAttribute('id', 'video-user-' + data.data.userId);
      box.ondblclick = () => {
        if (!box.fullscreen) {
          box.fullscreen = true;
          box.setAttribute('style', 'z-index:100;top:0;left:0;width:320px;height:180px;display:inline-block;width:100%;height: 100%;position: absolute;');
        } else {
          box.fullscreen = false;
          box.setAttribute('style', 'width:320px;height:180px;display:inline-block;');
        }
      };
      data.data.videoTag.setAttribute('style', 'width:100%;height:100%;');
      data.data.videoTag.onresize = function () { title.innerHTML = 'user:' + data.data.userName + '(' + data.data.userId + ') resolution:' + data.data.videoTag.videoWidth + 'x' + data.data.videoTag.videoHeight; };
      box.appendChild(data.data.videoTag);
      box.appendChild(title);
      activeVideoContainer.appendChild(box);
    }
  });

  rtcEngine.on(PanoRtc.RtcEngine.Events.userScreenReceived, (data) => {
    console.log('demo app: receive remote share,', data);
    if (data.data.videoTag) {
      if (data.data.videoTag.id === `video${data.data.userId}`) {
        console.log(`get remote video again video${data.data.userId}`);
        return;
      }
      data.data.videoTag.id = `video${data.data.userId}`;
      const box = document.createElement('div');
      const title = document.createElement('div');
      title.innerHTML = 'user:' + data.data.userName + '(' + data.data.userId + ')';
      title.setAttribute('style', 'width:320px;font-size:8px;opacity:0.6;top:-22px;left:0px; background:#00BCD4;position:relative');
      box.setAttribute('style', 'width:320px;height:180px;display:inline-block');
      box.setAttribute('id', 'video-user-' + data.data.userId);
      box.ondblclick = () => {
        if (!box.fullscreen) {
          box.fullscreen = true;
          box.setAttribute('style', 'z-index:100;top:0;left:0;width:320px;height:180px;display:inline-block;width:100%;height: 100%;position: absolute;');
        } else {
          box.fullscreen = false;
          box.setAttribute('style', 'width:320px;height:180px;display:inline-block;');
        }
      };
      data.data.videoTag.setAttribute('style', 'width:100%;height:100%;');
      data.data.videoTag.onresize = function () { title.innerHTML = 'user:' + data.data.userName + '(' + data.data.userId + ') resolution:' + data.data.videoTag.videoWidth + 'x' + data.data.videoTag.videoHeight; };
      box.appendChild(data.data.videoTag);
      box.appendChild(title);
      activeVideoContainer.appendChild(box);
    }
  });

  rtcEngine.on(PanoRtc.RtcEngine.Events.screenStopped, (data) => {
    button_share.innerHTML = 'Start Share';
    button_share.onclick = startShare;
  });

  rtcEngine.on(PanoRtc.RtcEngine.Events.getScreenMediaFailed, (data) => {
    button_share.innerHTML = 'Start Share';
    button_share.onclick = startShare;
    console.error(data);
  });

  rtcEngine.on(PanoRtc.RtcEngine.Events.userScreenStart, (data) => {
    userMediaStatusUpdate(data, 'share', 'unmute');
  });

  rtcEngine.on(PanoRtc.RtcEngine.Events.userScreenStop, (data) => {
    userMediaStatusUpdate(data, 'share', 'closed');
  });
  rtcEngine.on(PanoRtc.RtcEngine.Events.userScreenMute, (data) => {
    userMediaStatusUpdate(data, 'share', 'mute');
  });
  rtcEngine.on(PanoRtc.RtcEngine.Events.userScreenUnmute, (data) => {
    userMediaStatusUpdate(data, 'share', 'unmute');
  });

  rtcEngine.on(PanoRtc.RtcEngine.Events.userVideoMuted, (data) => {
    userMediaStatusUpdate(data, 'video', 'mute');
  });
  rtcEngine.on(PanoRtc.RtcEngine.Events.userVideoStart, (data) => {
    userMediaStatusUpdate(data, 'video', 'unmute');
  });

  rtcEngine.on(PanoRtc.RtcEngine.Events.userVideoUnmuted, (data) => {
    userMediaStatusUpdate(data, 'video', 'unmute');
  });
  rtcEngine.on(PanoRtc.RtcEngine.Events.userVideoStop, (data) => {
    const v = document.getElementById('video-user-' + data.userId);
    if (v) {
      activeVideoContainer.removeChild(v);
    }
    userMediaStatusUpdate(data, 'video', 'closed');
  });

  rtcEngine.on(PanoRtc.RtcEngine.Events.userAudioStart, (data) => {
    userMediaStatusUpdate(data, 'audio', 'unmute');
  });
  rtcEngine.on(PanoRtc.RtcEngine.Events.userAudioStop, (data) => {
    userMediaStatusUpdate(data, 'audio', 'closed');
  });
  rtcEngine.on(PanoRtc.RtcEngine.Events.userAudioMuted, (data) => {
    userMediaStatusUpdate(data, 'audio', 'mute');
  });

  rtcEngine.on(PanoRtc.RtcEngine.Events.userAudioUnmute, (data) => {
    userMediaStatusUpdate(data, 'audio', 'unmute');
  });

  rtcEngine.on(PanoRtc.RtcEngine.Events.getLocalVideo, (data) => {
    if (data.data.videoTag) {
      let label = document.createElement('p')
      label.innerHTML='myself'
      selfVideoContainer.appendChild(label);
      data.data.videoTag.setAttribute('style', 'width:150px;height:150px; background: black');
      selfVideoContainer.appendChild(data.data.videoTag);
    }
  });

  // init params
  PanoDemo.appId = document.getElementById('appID').value;
  PanoDemo.token = document.getElementById('token').value;
  PanoDemo.channelId = document.getElementById('channelID').value;
  PanoDemo.userId = document.getElementById('userID').value;
  PanoDemo.userName = document.getElementById('userName').value;
  document.querySelectorAll('input[name="channelMode"]').forEach((radio) => {
    if (radio.checked) {
      PanoDemo.channelMode =
        radio.value === 'meeting'
          ? PanoRtc.Constants.ChannelMode.TYPE_MEETING
          : PanoRtc.Constants.ChannelMode.TYPE_1_V_1
    }
  })
  if (
    !PanoDemo.appId ||
    !PanoDemo.channelId ||
    !PanoDemo.userId ||
    !PanoDemo.token
  ) {
    alert('请填写必要参数！')
    return
  }

  let channelParam = {
    appId: PanoDemo.appId,
    token: PanoDemo.token,
    channelId: PanoDemo.channelId,
    channelMode: PanoDemo.channelMode,
    userId: PanoDemo.userId,
    userName: PanoDemo.userName,
    subscribeAudioAll: true
  };
  const joinChannelResult = rtcEngine.joinChannel(channelParam, {
    joinChannelType: PanoRtc.Constants.JoinChannelType.mediaOnly
  });
  console.log('joinChannelResult: ', joinChannelResult);
} 


function leaveChannel(passive = false) {
  button_leaveChannel.disabled = true;
  button_leaveChannel.style.color = 'black';
  init_Btn_Stat();
  if (!passive) {
    rtcEngine.leaveChannel();
  }
  PanoDemo = {
    users: []
  };
  let rtcServer = document.getElementById('rtcServer').value
  rtcEngine = new PanoRtc.RtcEngine({
    appId,
    rtcServer: rtcServer || 'https://aisle.pano.video'
  });
  window.rtcEngine = rtcEngine;
  button_joinChannel.disabled = false;
  button_joinChannel.style.color = 'green';
  textArea_roster.value = '';

  selfVideoContainer.innerHTML = '';
  activeVideoContainer.innerHTML = '';

  setTimeout(function () {
    // location.reload(true);
  }, 2000);
}

function updateRoster() {
  if (PanoDemo.users instanceof Array) {
    let list = '';
    video_user_list.innerHTML = '';
    share_user_list.innerHTML = '';
    PanoDemo.users.forEach(function (user) {
      list += 'Name: ' + user.userName + ', ID: ' + user.userId + ' \r\n' +
        'Audio: ' + (user.audioStatus ? user.audioStatus : 'closed') +
        ', Video: ' + (user.videoStatus ? user.videoStatus : 'closed') +
        ', Share:  ' + (user.screenStatus ? user.screenStatus : 'closed') + '\r\n \r\n';
      if (user.videoStatus === 'open' || user.videoStatus === 'unmute') {
        let option = document.createElement('option');
        option.text = user.userName;
        option.value = user.userId;
        video_user_list.add(option);
      }
      if (user.screenStatus === 'open' || user.screenStatus === 'unmute') {
        let option = document.createElement('option');
        option.text = user.userName;
        option.value = user.userId;
        share_user_list.add(option);
      }
    });
    textArea_roster.value = list;
  }
}

function userMediaStatusUpdate (data, kind, status) {
  console.log('demo app: receive user video status update,', data);
  if (PanoDemo.users instanceof Array) {
    for (let i = 0; i < PanoDemo.users.length; i++) {
      if (PanoDemo.users[i].userId === data.userId) {
        if (kind === 'audio') {
          PanoDemo.users[i].audioStatus = status;
        } else if (kind === 'video') {
          PanoDemo.users[i].videoStatus = status;
          if (!PanoDemo.users[i].subscribedVideo && (PanoDemo.users[i].videoStatus === 'open' || PanoDemo.users[i].videoStatus === 'unmute')) {
            PanoDemo.users[i].subscribedVideo = true;
            subscribeVideo(null, PanoDemo.users[i].userId);
          } else if (PanoDemo.users[i].subscribedVideo && (PanoDemo.users[i].videoStatus === 'closed' || PanoDemo.users[i].videoStatus === 'mute')) {
            PanoDemo.users[i].subscribedVideo = false;
            unSubscribeVideo(null, PanoDemo.users[i].userId);
          }
        } else if (kind === 'share') {
          PanoDemo.users[i].screenStatus = status;
        }
        break;
      }
    }
    updateRoster();
  }
}


function pano_getMics() {
  rtcEngine.getMics(sucesss, fail);

  function sucesss(devices) {
    let length = select_mic.options.length;
    for (let i = length - 1; i >= 0; i--) {
      select_mic.options[i] = null;
    }

    for (let i = 0; i < devices.length; i++) {
      let option = document.createElement('option');
      option.text = devices[i].label;
      select_mic.add(option);
    }
  }
  function fail() {
    console.error('Failed to get mics.');
  }
}

function pano_getSpeakers() {
  rtcEngine.getSpeakers(sucesss, fail);

  function sucesss(devices) {
    let length = select_speaker.options.length;
    for (let i = length - 1; i >= 0; i--) {
      select_speaker.options[i] = null;
    }

    for (let i = 0; i < devices.length; i++) {
      let option = document.createElement('option');
      option.text = devices[i].label;
      select_speaker.add(option);
    }
  }

  function fail () {
    console.error('Failed to get speakers.');
  }
}

function pano_getCams () {
  rtcEngine.getCams(sucesss, fail);

  function sucesss (devices) {
    let length = select_cam.options.length;
    for (let i = length - 1; i >= 0; i--) {
      select_cam.options[i] = null;
    }

    for (let i = 0; i < devices.length; i++) {
      let option = document.createElement('option');
      option.text = devices[i].label;
      option.value = devices[i].deviceId;
      select_cam.add(option);
    }
  }

  function fail () {
    console.error('Failed to get cams.');
  }
}

function pano_setCam () {
  rtcEngine.selectCam(select_cam.value);
}

function pano_muteMic () {
  rtcEngine.muteMic();
  button_mute_mic.innerHTML = 'Unmute Mic';
  button_mute_mic.onclick = pano_unmuteMic;
}

function pano_unmuteMic () {
  rtcEngine.unmuteMic();
  button_mute_mic.innerHTML = 'Mute Mic';
  button_mute_mic.onclick = pano_muteMic;
}

function pano_muteScreen () {
  rtcEngine.muteScreen();
  button_mute_screen.innerHTML = 'Unmute Screen';
  button_mute_screen.onclick = pano_unmuteScreen;
}

function pano_unmuteScreen () {
  rtcEngine.unmuteScreen();
  button_mute_screen.innerHTML = 'Mute Screen';
  button_mute_screen.onclick = pano_muteScreen;
}

function startAudio () {
  rtcEngine.startAudio();
  button_audio.innerHTML = 'Stop Audio';
  button_audio.onclick = stopAudio;
}

function stopAudio () {
  rtcEngine.stopAudio();
  button_audio.innerHTML = 'Start Audio';
  button_audio.onclick = startAudio;
}

function startVideo () {
  rtcEngine.startVideo();
  button_video.innerHTML = 'Stop Video';
  button_video.onclick = stopVideo;
}

function stopVideo () {
  rtcEngine.stopVideo();
  selfVideoContainer.innerHTML = '';
  button_video.innerHTML = 'Start Video';
  button_video.onclick = startVideo;
}

function startShare () {
  rtcEngine.startScreen();
  button_share.innerHTML = 'Stop Screen';
  button_share.onclick = stopShare;
}

function stopShare () {
  rtcEngine.stopScreen();
  button_share.innerHTML = 'Start Screen';
  button_share.onclick = startShare;
}

function subscribeVideo (e, userId) {
  if (!userId) {
    let index = video_user_list.selectedIndex;
    if (index >= 0 && index < video_user_list.length) {
      userId = (video_user_list[index].value);
    }
  }

  let user = null;
  if (userId && PanoDemo.users instanceof Array) {
    for (let i = 0; i < PanoDemo.users.length; i++) {
      if (PanoDemo.users[i].userId === userId && (PanoDemo.users[i].videoStatus === 'open' || PanoDemo.users[i].videoStatus === 'unmute')) {
        user = PanoDemo.users[i];
        break;
      }
    }
  }
  if (user) {
    const { VideoProfileType } = PanoRtc.Constants
    const quality = sub_video_quality.options[sub_video_quality.selectedIndex].value
    let params = {
      userId: user.userId,
      quality: VideoProfileType[quality],
      userName: user.userName
    };

    rtcEngine.subscribeVideo(params);
  }
}

function unSubscribeVideo (e, userId) {
  if (!userId) {
    let index = video_user_list.selectedIndex;
    if (index >= 0 && index < video_user_list.length) {
      userId = (video_user_list[index].value);
    }
  }

  let user = null;
  if (userId && PanoDemo.users instanceof Array) {
    for (let i = 0; i < PanoDemo.users.length; i++) {
      if (PanoDemo.users[i].userId === userId) {
        user = PanoDemo.users[i];
        break;
      }
    }
  }
  if (user) {
    rtcEngine.unsubscribeVideo(user);
    let v = document.getElementById('video-user-' + user.userId);
    if (v) {
      activeVideoContainer.removeChild(v);
    }
  }
}
function pano_snapshotMyself() {
  let show = file => {
    console.log(`got file ${file}`);
    let img = document.createElement('img');
    img.setAttribute('style', 'width:110px; height:70px;');
    if (typeof file === 'string') {
      img.src = `data:image/png;base64,${file}`;
    } else {
      let url = URL.createObjectURL(file);
      img.src = url;
      img.onload = () => {
        URL.revokeObjectURL(url);
      };
    }
    document.getElementById('snapshotImg').innerHTML = '';
    let label = document.createElement('p')
    label.innerHTML = 'snapshot'
    document.getElementById('snapshotImg').appendChild(label);
    document.getElementById('snapshotImg').appendChild(img);
  };
  rtcEngine.snapshotVideo(
    {
      userId: document.getElementById('snapshotTarget').value,
      imgObjType: document.getElementById('snapshotImgObjType').value
    }, show, console.log);
}
function subscribeShare (e, userId) {
  if (!userId) {
    let index = share_user_list.selectedIndex;
    if (index >= 0 && index < share_user_list.length) {
      userId = (share_user_list[index].value);
    }
  }

  let user = null;
  if (userId && PanoDemo.users instanceof Array) {
    for (let i = 0; i < PanoDemo.users.length; i++) {
      if (PanoDemo.users[i].userId === userId && (PanoDemo.users[i].screenStatus === 'open' || PanoDemo.users[i].screenStatus === 'unmute')) {
        user = PanoDemo.users[i];
        break;
      }
    }
  }
  if (user) {
    let params = {
      userId: user.userId,
      userName: user.userName
    };

    rtcEngine.subscribeScreen(params);
  }
}

function unSubscribeShare (e, userId) {
  if (!userId) {
    let index = share_user_list.selectedIndex;
    if (index >= 0 && index < share_user_list.length) {
      userId = (share_user_list[index].value);
    }
  }

  let user = null;
  if (userId && PanoDemo.users instanceof Array) {
    for (let i = 0; i < PanoDemo.users.length; i++) {
      if (PanoDemo.users[i].userId === userId) {
        user = PanoDemo.users[i];
        break;
      }
    }
  }
  if (user) {
    rtcEngine.unsubscribeScreen(user);
    let v = document.getElementById('video-user-' + user.userId);
    if (v) {
      activeVideoContainer.removeChild(v);
    }
  }
}

function pano_getPreview () {
  if (!select_cam.value) {
    console.log('no selected cam, please get cam list first');
    return;
  }
  rtcEngine.startPreview(select_cam.value, vtag => {
    vtag.setAttribute('style', 'width:150px;height:150px');
    previewVideoContainer.appendChild(vtag);

    let label = document.createElement('p');
    label.setAttribute('style', 'position:absolute; top:0px;');
    label.innerHTML = 'preview';
    previewVideoContainer.appendChild(label);
  });
  button_get_preview.innerHTML = 'Stop View';
  button_get_preview.onclick = pano_stopPreview;
}

function pano_stopPreview () {
  rtcEngine.stopPreview(previewVideoContainer.firstChild);
  while (previewVideoContainer.hasChildNodes()) {
    previewVideoContainer.removeChild(previewVideoContainer.firstChild);
  }
  button_get_preview.innerHTML = 'Get Preview';
  button_get_preview.onclick = pano_getPreview;
  // selfVideoContainer.innerHTML = '';
}

function switchFullScreen () {
  if (!fullscreen) {
    fullscreen = true;
    activeVideoContainer.setAttribute('style', 'width:100%;height: 100%;position: absolute;top:0;left:0');
  } else {
    fullscreen = false;
    activeVideoContainer.setAttribute('style', 'width:100%;height: 100%;');
  }
}


(function () {
  let rand = Math.random();
  let userId = '1908' + Math.round(rand * 9000);
  document.getElementById('userID').value = userId;
  document.getElementById('userName').value = 'Alice-' + userId;
  init_UI();
})();
