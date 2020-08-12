// Modules to control application life and create native browser window
'use strict';

const {app, ipcMain, BrowserWindow} = require('electron');
const path = require('path');
const nodeConsole = require('console');
var myConsole = new nodeConsole.Console(process.stdout, process.stderr);
var mainWindow;

function createWindow () {
  // Create the browser window.
  mainWindow = new BrowserWindow({
    width: 1200,
    height: 640,
    webPreferences: {
      nodeIntegration: true,
      preload: path.join(__dirname, 'preload.js')
    }
  });
  if (process.platform === "win32") {
    mainWindow.setMenu(null);
  }

  // and load the index.html of the app.
  mainWindow.loadFile('./src/ui/index.html');
  mainWindow.webContents.openDevTools();

  mainWindow.webContents.on('did-finish-load', () => {
    myConsole.log('+++++ JS mainWindow did-finish-load');
  });
  mainWindow.webContents.on('devtools-closed', () => {
    mainWindow.webContents.send('devtools-closed');
  });

  mainWindow.on('closed', function () {
    // Dereference the window object, usually you would store windows
    // in an array if your app supports multi windows, this is the time
    // when you should delete the corresponding element.
    mainWindow = null;
    myConsole.log('+++++ JS mainWindow closed');
  });
}

// This method will be called when Electron has finished
// initialization and is ready to create browser windows.
// Some APIs can only be used after this event occurs.
app.whenReady().then(() => {
  createWindow()
  
  app.on('activate', function () {
    // On macOS it's common to re-create a window in the app when the
    // dock icon is clicked and there are no other windows open.
    if (BrowserWindow.getAllWindows().length === 0) createWindow()
  });
});

// Quit when all windows are closed, except on macOS. There, it's common
// for applications and their menu bar to stay active until the user quits
// explicitly with Cmd + Q.
app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') app.quit()
});

ipcMain.on('toggle-devtools', (event, args) => {
  mainWindow.webContents.toggleDevTools();
});
