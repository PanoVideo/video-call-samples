# PanoRtc-Electron-SDK-Demo

## Installing SDK

`npm install @pano.video/panortc-electron-sdk`

## RtcEngine 连接步骤

> 连接步骤可以参考 [sample](https://github.com/PanoVideo/audio-call-samples/tree/master/Web)

[RtcEngine 接口文档](https://developer.pano.video/sdk/websdk/websdk_rtc/)

连接步骤：

1. 获取`PanoRtc`实例: `const {rtcEngine} = require('@pano.video/panortc-electron-sdk')`
2. [获取 PanoToken](https://developer.pano.video/restful/generatetoken/)
3. 调用 `rtcEngine.initialize` , 初始化 PanoRtcEngine
4. 调用 `rtcEngine.joinChannel` , 使用第二步获取的 `panoToken` 和会议 `channelId` 等参数加入会议，可以设置 serviceFlags 类型控制只加入音视频频道、白板频道或者二者都加入
5. 监听到 `channelJoinConfirm` 事件后即可开始通信

示例代码：

```javascript
const {rtcEngine} = require('@pano.video/panortc-electron-sdk')

rtcEngine.on('channelJoinConfirm', (result) => {
  console.log(`join channel ${result}`);
})
rtcEngine.initialize(appId, { // engine options, 可不填
  videoHwAccel: false, 
  audioAecType: 1, 
  audioScenario: 0
})
rtcEngine.joinChannel(token, channelId, userId, { // channel options, 可不填
  channelMode: 1,  // 0 - peer-to-peer, 1 - meeting
  serviceFlags: 1, // 1 - media, 2 - whiteboard, 3 - media + whiteboard
  subscribeAudioAll: true, // subscribe user audio automatically
  userName: 'xxxx'
})

```
