import Vue from 'vue'
import Toolbar from './Toolbar.vue'

let toolbarInstance

export default {
  install(whiteboard) {
    this.unInstall()
    toolbarInstance = new Vue({
      render(h) {
        return h(Toolbar, { props: { whiteboard } })
      }
    })
    toolbarInstance.$mount(whiteboard.toolsWrapper)
  },
  unInstall() {
    toolbarInstance && toolbarInstance.$destroy()
  }
}
