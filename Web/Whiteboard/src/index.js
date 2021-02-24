/**
 * 使用 @pano.video/panortc 使用白板的示例
 */
import Panortc from '@pano.video/panortc'
// import Toolbar from './toolbar-react'
import Toolbar from './toolbar-vue'

const joinForm = document.getElementById('joinForm')
const joinBtn = document.getElementById('joinBtn')
const whiteboardWrapper = document.getElementById('whiteboardWrapper')
const panoControlPanel = document.getElementById('panoControlPanel')
const preBtnText = joinBtn.innerHTML
const openWhiteboardBtn = document.getElementById('openWhiteboardBtn')
const userIdInput = document.getElementById('userId')
const userNameInput = document.getElementById('userName')

userIdInput.value = Math.floor(Math.random() * 10000000) + ''
userNameInput.value = `pano-${userIdInput.value}`

let rtcWhiteboard
function onOpenWhiteboard(whiteboardClient) {
  if (!whiteboardClient) {
    console.log('open whiteboard failed, no whiteboard instance.')
    return
  }
  whiteboardClient.open(whiteboardWrapper)
  panoControlPanel.style.display = 'none'
  whiteboardWrapper.style.display = 'block'
  whiteboardClient && Toolbar.install(whiteboardClient)
}
const rtcEngine = new Panortc.RtcEngine(document.getElementById('appId'))

joinBtn.onclick = async () => {
  if (rtcWhiteboard) {
    onOpenWhiteboard(rtcWhiteboard)
    return
  }
  joinBtn.disabled = true
  joinBtn.innerHTML = 'Connecting...'
  const joinParams = {}
  Array.from(joinForm.getElementsByTagName('input')).forEach((item) => {
    joinParams[item.name] = item.value
  })
  const { appId, channelId, userId, name, panoToken } = joinParams

  rtcEngine.joinChannel(
    {
      appId,
      token: panoToken,
      channelId,
      channelMode: PanoRtc.Constants.ChannelMode.TYPE_MEETING,
      userId,
      userName: name
    },
    {
      joinChannelType: PanoRtc.Constants.JoinChannelType.whiteboardOnly
    }
  )
}

rtcEngine.on(PanoRtc.RtcEngine.Events.joinChannelConfirm, (data) => {
  if (data.result !== 'success') {
    window.alert(`join channel failed because: ${data.message}`)
    return
  }
  console.log('join channel success!')
})

rtcEngine.on(PanoRtc.RtcEngine.Events.whiteboardAvailable, (data) => {
  console.log('demo app: whiteboardAvailable', data)
  rtcWhiteboard = rtcEngine.getWhiteboard()
  window.rtcWhiteboard = rtcWhiteboard
  console.log('whiteboardClient connect success')
  // 隐藏 join channel 面板
  joinForm.style.display = 'none'
  joinBtn.disabled = false
  joinBtn.innerHTML = preBtnText

  // 显示 pano 控制面板
  // 在这个面板里可以手动打开 whiteboard
  panoControlPanel.style.display = 'block'
  // 添加处理打开面板的方法，这个方法会在收到打开面板指令时自动调用
  // 正常情况下，如果用户加入的会议里已经有其他用户打开白板，那么别的用户会在刚加入 channel 时立即收到打开白板的指令
  // 如果没有人打开过白板，那么用户可以手动打开白板，sdk 会自动发送打开指令给别的 client
  openWhiteboardBtn.addEventListener('click', () =>
    onOpenWhiteboard(rtcWhiteboard)
  )

  rtcWhiteboard.on(Panortc.RtcWhiteboard.Events.openStateChanged, () =>
    onOpenWhiteboard(rtcWhiteboard)
  )

  rtcWhiteboard.on(Panortc.RtcWhiteboard.Events.messageReceived, (data) => {
    console.log('receive custom message:', data)
  })
  rtcWhiteboard.on(
    Panortc.RtcWhiteboard.Events.whiteboardFailover,
    (payload) => {
      console.error('got failover event', payload)
      if (payload.state === 'Success') {
        onOpenWhiteboard(rtcWhiteboard)
      }
    }
  )

  rtcWhiteboard.on(
    Panortc.RtcWhiteboard.Events.readyStateChanged,
    (payload) => {
      console.log(
        'demo app readyStateChanged: ',
        payload.ready,
        ',message: ',
        payload.message
      )
      if (!payload.ready) {
        alert(payload.message)
      }
    }
  )
})

rtcEngine.on(PanoRtc.RtcEngine.Events.whiteboardUnavailable, (data) => {
  console.log('demo app: whiteboardUnavailable', data)
  whiteboardWrapper.style.display = 'none'
  panoControlPanel.style.display = 'none'
})

console.log(
  '%c @pano.video/panortc 您正在使用panortc完整版sdk.',
  'color: blue; font-weight: bold'
)
