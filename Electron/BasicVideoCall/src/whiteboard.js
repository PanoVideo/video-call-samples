
'use strict';

; (function (window, undefined) {
  const {QResult, WBRoleType, WBToolType} = require('@pano.video/panortc-electron-sdk/js/panodefs');
  const nodeConsole = require('console');
  const myConsole = new nodeConsole.Console(process.stdout, process.stderr);

  var rtcWhiteboard;

  function initWhiteboard(whiteboard) {
    rtcWhiteboard = whiteboard;

    rtcWhiteboard.on('pageNumberChanged', () => {
      myConsole.log('+++++ JS onPageNumberChanged');
    });
    rtcWhiteboard.on('imageStateChanged', () => {
      myConsole.log('+++++ JS onImageStateChanged');
    });
    rtcWhiteboard.on('viewScaleChanged', () => {
      myConsole.log('+++++ JS onViewScaleChanged');
    });
    rtcWhiteboard.on('roleTypeChanged', () => {
      myConsole.log('+++++ JS onRoleTypeChanged');
    });
    rtcWhiteboard.on('contentUpdated', () => {
      //myConsole.log('+++++ JS onContentUpdated');
    });
    rtcWhiteboard.on('snapshotComplete', () => {
      myConsole.log('+++++ JS onSnapshotComplete');
    });
    rtcWhiteboard.on('message', () => {
      myConsole.log('+++++ JS onMessage');
    });
    rtcWhiteboard.on('addBackgroundImages', () => {
      myConsole.log('+++++ JS onAddBackgroundImages');
    });
    rtcWhiteboard.on('docTranscodeStatus', () => {
      myConsole.log('+++++ JS onDocTranscodeStatus');
    });
    rtcWhiteboard.on('createDoc', () => {
      myConsole.log('+++++ JS onCreateDoc');
    });
    rtcWhiteboard.on('deleteDoc', () => {
      myConsole.log('+++++ JS onDeleteDoc');
    });
    rtcWhiteboard.on('switchDoc', () => {
      myConsole.log('+++++ JS onSwitchDoc');
    });
    rtcWhiteboard.on('drawRectUpdate', () => {
      myConsole.log('+++++ JS onDrawRectUpdate');
    });
  }

  function openWhiteboard(handle) {
    myConsole.log('+++++ JS WB openWhiteboard');
    rtcWhiteboard.setToolType(WBToolType.PATH);
    rtcWhiteboard.setLineWidth(10);
    rtcWhiteboard.setFontSize(36);
    rtcWhiteboard.setForegroundColor(128, 38, 84);
    rtcWhiteboard.setRoleType(WBRoleType.ADMIN);
    rtcWhiteboard.open(handle);
    rtcWhiteboard.setBackgroundImage('https://www.pano.video/assets/img/backbone.png');
  }

  function closeWhiteboard() {
    rtcWhiteboard.close();
  }

  function stopWhiteboard() {
    rtcWhiteboard = null;
  }


  module.exports = {
    initWhiteboard: initWhiteboard,
    openWhiteboard: openWhiteboard,
    closeWhiteboard: closeWhiteboard,
    stopWhiteboard: stopWhiteboard
  }

})(window);
