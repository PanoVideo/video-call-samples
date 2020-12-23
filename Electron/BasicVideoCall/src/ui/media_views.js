
'use strict';

; (function (window, undefined) {

  class ViewInfo {
    constructor(isMain) {
      this.userId = null;
      this.view = null;
      this.isFree = true;
      this.isMain = isMain;
      this.isScreen = false;
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

      for (let i=0; i<5; ++i) {
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
        this.mainViewInfo.isScreen = false;
      }
      for (let vi of this.remoteViewInfoList) {
        if (vi.view) {
          vi.view = null;
          vi.isFree = true;
        }
      }
      this.remoteViewInfoList = [];
    }
  
    getMainViewInfo() {
      return this.mainViewInfo;
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
  
    getRemoteUserVideoView(userId) {
      let vi = this.getRemoteUserViewInfo(userId);
      if (vi) {
        return vi.view;
      }
  
      return null;
    }
  
    getRemoteUserVideoViewInfo(userId) {
      if (!this.mainViewInfo.isFree && 
          this.mainViewInfo.userId === userId &&
          !this.mainViewInfo.isScreen) {
        return this.mainViewInfo;
      }
      for (let vi of this.remoteViewInfoList) {
        if (!vi.isFree && vi.userId === userId) {
          return vi;
        }
      }
  
      return null;
    }
  
    allocRemoteUserVideoView(userId) {
      let vi = this.getRemoteUserVideoViewInfo(userId);
      if (vi != null) {
        return {view: vi.view, isMain: vi.isMain};
      }
      if (this.mainViewInfo.isFree && this.mainViewInfo.view != null) {
        this.mainViewInfo.userId = userId;
        this.mainViewInfo.isFree = false;
        this.mainViewInfo.isScreen = false;
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
  
    showRemoteUserVideoView(userId) {
      if (!this.mainViewInfo.isFree && 
          this.mainViewInfo.userId === userId &&
          !this.mainViewInfo.isScreen) {
        return ;
      }
      let vi = this.getRemoteUserVideoViewInfo(userId);
      if (vi && vi.view) {
        vi.view.style.display = 'inline-block';
      }
    }
  
    stopRemoteUserVideoView(userId) {
      if (!this.mainViewInfo.isFree && 
          this.mainViewInfo.userId === userId &&
          !this.mainViewInfo.isScreen) {
        this.mainViewInfo.userId = null;
        this.mainViewInfo.isFree = true;
        this.mainViewInfo.isScreen = false;
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

    stopRemoteUserScreenView(userId) {
      if (!this.mainViewInfo.isFree && 
          this.mainViewInfo.userId === userId &&
          this.mainViewInfo.isScreen) {
        this.mainViewInfo.userId = null;
        this.mainViewInfo.isFree = true;
        this.mainViewInfo.isScreen = false;
      }
    }

    stopRemoteUserViews(userId) {
      if (!this.mainViewInfo.isFree && 
          this.mainViewInfo.userId === userId &&
          !this.mainViewInfo.isScreen) {
        this.mainViewInfo.userId = null;
        this.mainViewInfo.streamId = null;
        this.mainViewInfo.isFree = true;
        this.mainViewInfo.isScreen = false;
        console.log('+++++ JS stopRemoteUserViews,main view, userId: ' + userId);
      }
      for (let vi of this.remoteViewInfoList) {
        if (!vi.isFree && vi.userId === userId) {
          vi.userId = null;
          vi.streamId = null;
          vi.isFree = true;
          vi.view.style.display = 'none';
          console.log('+++++ JS stopRemoteUserViews,remote view, userId: ' + userId);
        }
      }
    }
  }


  module.exports = 
  {
    viewMgr: new ViewManager()
  };

})(window);
