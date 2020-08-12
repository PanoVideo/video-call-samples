
'use strict';

; (function (window, undefined) {

  class ViewInfo {
    constructor(isMain) {
      this.userId = null;
      this.view = null;
      this.isFree = true;
      this.isMain = isMain;
    }
  
    getUserId() {
      return this.userId;
    }
    getView() {
      return this.view;
    }
  }

  class ViewManager {
    constructor() {
      this.localViewInfo = new ViewInfo();
      this.mainViewInfo = new ViewInfo(true);
      this.remoteViewInfoList = [];
    }

    initViews() {
      this.mainViewInfo.view = document.getElementById('div_video_streams');
      this.localViewInfo.view = document.getElementById('div_local_video_stream');

      for (var i=0; i<5; ++i) {
        let view = document.getElementById('div_remote_video_stream_' + (i+1));
        if (view) {
          this.remoteViewInfoList[i] = new ViewInfo();
          this.remoteViewInfoList[i].view = view;
        } else {
          break;
        }
      }
    }

    freeViews() {
      this.localViewInfo.isFree = true;
      this.mainViewInfo.isFree = true;
      for (let vi of this.remoteViewInfoList) {
        vi.isFree = true;
      }
    }

    closeViews() {
      if (this.localViewInfo.view) {
        this.localViewInfo.view = null;
      }
      if (this.mainViewInfo.view) {
        this.mainViewInfo.isFree = true;
        this.mainViewInfo.view = null;
      }
      for (let vi of this.remoteViewInfoList) {
        if (vi.view) {
          vi.view = null;
          vi.isFree = true;
        }
      }
      this.remoteViewInfoList = [];
    }
  
    getMainView() {
      return this.mainViewInfo.view;
    }
  
    getLocalView() {
      return this.localViewInfo.view;
    }
  
    showLocalView() {
      if (this.localViewInfo.view) {
        this.localViewInfo.view.style.display = 'inline-block';
      }
    }
  
    stopLocalView() {
      if (this.localViewInfo.view) {
        this.localViewInfo.view.style.display = 'none';
      }
    }
  
    getRemoteUserView(userId) {
      let vi = this.getRemoteUserViewInfo(userId);
      if (vi) {
        return vi.view;
      }
  
      return null;
    }
  
    getRemoteUserViewInfo(userId) {
      if (!this.mainViewInfo.isFree && this.mainViewInfo.userId === userId) {
        return this.mainViewInfo;
      }
      for (let vi of this.remoteViewInfoList) {
        if (!vi.isFree && vi.userId === userId) {
          return vi;
        }
      }
  
      return null;
    }
  
    allocRemoteUserView(userId) {
      let vi = this.getRemoteUserViewInfo(userId);
      if (vi != null) {
        return {view: vi.view, isMain: vi.isMain};
      }
      if (this.mainViewInfo.isFree && this.mainViewInfo.view != null) {
        this.mainViewInfo.userId = userId;
        this.mainViewInfo.isFree = false;
        return {view: this.mainViewInfo.view, isMain: true};
      }
      for (let vi of this.remoteViewInfoList) {
        if (vi.isFree && vi.view != null) {
          vi.userId = userId;
          vi.isFree = false;
          return {view: vi.view, isMain: false};
        }
      }
      return null;
    }
  
    showRemoteUserView(userId) {
      if (!this.mainViewInfo.isFree && this.mainViewInfo.userId === userId) {
        return ;
      }
      let vi = this.getRemoteUserViewInfo(userId);
      if (vi && vi.view) {
        vi.view.style.display = 'inline-block';
      }
    }
  
    stopRemoteUserView(userId) {
      if (!this.mainViewInfo.isFree && this.mainViewInfo.userId === userId) {
        this.mainViewInfo.userId = null;
        this.mainViewInfo.isFree = true;
        return;
      }
      for (let vi of this.remoteViewInfoList) {
        if (!vi.isFree && vi.userId === userId) {
          vi.userId = null;
          vi.isFree = true;
          vi.view.style.display = 'none';
          break;
        }
      }
    }
  }


  module.exports = 
  {
    viewMgr: new ViewManager()
  };

})(window);
