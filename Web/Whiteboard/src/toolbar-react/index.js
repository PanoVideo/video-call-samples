import React from 'react'
import ReactDOM from 'react-dom'
import Toolbar from './Toolbar.jsx'

export default {
  install(whiteboard) {
    ReactDOM.render(
      <Toolbar whiteboard={whiteboard} />,
      whiteboard.toolsWrapper
    )
  },
  unInstall(whiteboard) {
    ReactDOM.unmountComponentAtNode(whiteboard.toolsWrapper)
  }
}
