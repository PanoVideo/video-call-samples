# PanoRtc-Electron-SDK-Demo

## Installing SDK

`npm install @pano.video/panortc-electron-sdk`

## RtcEngine 连接步骤

[RtcEngine 接口文档](https://developer.pano.video/sdk/websdk/websdk_rtc/)

连接步骤：

1. 获取`PanoRtc`接口类: `const {RtcEngine} = require('@pano.video/panortc-electron-sdk')`
2. 创建`PanoRtc`接口对象: `var rtcEngine = new RtcEngine();`
3. [获取 PanoToken](https://developer.pano.video/restful/generatetoken/)
4. 调用 `rtcEngine.initialize` , 初始化 PanoRtcEngine
5. 调用 `rtcEngine.joinChannel` , 使用第二步获取的 `panoToken` 和会议 `channelId` 等参数加入会议，可以设置 serviceFlags 类型控制只加入音视频频道、白板频道或者二者都加入
6. 监听到 `channelJoinConfirm` 事件后即可开始通信

示例代码：

```javascript

const {
  RtcEngine,
  QResult, 
  ChannelMode,
  AudioAecType,
  kChannelServiceMedia
} = require('@pano.video/panortc-electron-sdk')

var rtcEngine = new RtcEngine();

rtcEngine.on('channelJoinConfirm', (result) => {
  console.log(`join channel ${result}`)
})
rtcEngine.initialize(appId, { // engine options, 可不填
  panoServer: 'api.pano.video', 
  videoHwAccel: false, 
  audioAecType: AudioAecType.Default, 
  audioScenario: 0
})
let serviceFlags = kChannelServiceMedia;
rtcEngine.joinChannel(token, channelId, userId, { // channel options, 可不填
  channelMode: ChannelMode.Mode_Meeting,
  serviceFlags: serviceFlags, // 1 - media, 2 - whiteboard, 3 - media + whiteboard
  subscribeAudioAll: true, // subscribe user audio automatically
  userName: 'xxxx'
})

```

## Q/A
#### clear all engine event listeners
```
rtcEngine.removeAllListeners()
```

#### VUE Webpack load node native addon
- add dependencies: native-ext-loader
- set webPreferences: { nodeIntegration: true }
- add webpack config:
```
{
    module: {
        rules: [
        {
            test: /\.node$/,
            loader: 'native-ext-loader',
            options: {
            emit: true,
            rewritePath: param.ENV === 'production'
                ? param.PLATFORM === 'win32' ? './resources' : '../Resources'
                : './node_modules/@pano.video/panortc-electron-sdk/native'
            }
        }
        ],
    }
}
```
- add following build config to package.json:
```
"extraResources": [
    {
        "from": "node_modules/@pano.video/panortc-electron-sdk/native/",
        "to": "./",
        "filter": [
            "**/*"
        ]
    }
]
```

#### integration PANO ScreenSource with Electron DesktopCapturerSource
Electron source ID 'window:10618822:1', the ID '10618822' can set to PANO as SSID

#### change dom view for video
call startVideo/subscribeVideo with new dom view

#### crashed on startVideo() when run in vscode console
don't run demo in vscode, otherwise the camera privilege dialog won't popup.