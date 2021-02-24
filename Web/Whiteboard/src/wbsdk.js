/**
 * 使用 @pano.video/whiteboard 使用白板的示例
 */
import * as PanoRtc from '@pano.video/whiteboard'
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

const rtcWhiteboard = new PanoRtc.RtcWhiteboard()
window.rtcWhiteboard = rtcWhiteboard

rtcWhiteboard.on(PanoRtc.RtcWhiteboard.Events.messageReceived, (data) => {
  console.log('receive custom message:', data)
})

rtcWhiteboard.on(PanoRtc.RtcWhiteboard.Events.whiteboardFailover, (payload) => {
  console.error('got failover event', payload)
})

rtcWhiteboard.on(PanoRtc.RtcWhiteboard.Events.readyStateChanged, (payload) => {
  console.log(
    'demo app readyStateChanged: ',
    payload.ready,
    ',message: ',
    payload.message
  )
})

rtcWhiteboard.on(PanoRtc.RtcWhiteboard.Events.openStateChanged, () =>
  onOpenWhiteboard()
)

function onOpenWhiteboard() {
  rtcWhiteboard.open(whiteboardWrapper)
  panoControlPanel.style.display = 'none'
  whiteboardWrapper.style.display = 'block'
  Toolbar.install(rtcWhiteboard)
}

joinBtn.onclick = async () => {
  if (rtcWhiteboard.isReady) {
    onOpenWhiteboard()
    return
  }
  joinBtn.disabled = true
  joinBtn.innerHTML = 'Connecting...'
  const joinParams = {}
  Array.from(joinForm.getElementsByTagName('input')).forEach((item) => {
    joinParams[item.name] = item.value
  })
  const { appId, channelId, userId, name, panoToken } = joinParams

  rtcWhiteboard.joinChannel(
    {
      appId,
      panoToken,
      channelId,
      name,
      userId
    },
    () => {
      console.log('whiteboardClient connect success!')
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
      openWhiteboardBtn.addEventListener('click', () => onOpenWhiteboard())
    },
    (result) => {
      joinBtn.disabled = false
      joinBtn.innerHTML = preBtnText
      console.log('channel join failed', result)
    }
  )
}

console.log(
  '%c @pano.video/whiteboard 您正在使用独立白板sdk.',
  'color: blue; font-weight: bold'
)
