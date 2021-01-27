# 拍乐云实时互动白板/视频 Demo 使用指导

## 1. 注册拍乐云账号
访问拍乐云 [注册账号](https://console.pano.video/#/user/register) 页面，根据指导创建拍乐云账号。

## 2. 创建应用
登录拍乐云 [控制台](https://console.pano.video/) ，进入应用管理页面创建一个新的应用，获得 APPID，APPID 用以区分不同的实时音视频应用。

## 3. 下载 Demo 源码
拍乐云Demo源代码托管在 [github](https://github.com/PanoVideo) 上，用户可从github上clone。

## 4. 生成临时token
用户在创建应用获取到 APPID 后，还需要 token 才可以使用拍乐云实时音视频服务。对于 Demo 用户可在拍乐云控制台为应用 <a href="https://developer.pano.video/getting-started/firstapp/#14-%E7%94%9F%E6%88%90%E4%B8%B4%E6%97%B6token">生成临时token</a>。

## 5. 安装依赖
在 BasicVideoCall 或 Whiteboard 目录下，运行 `npm i`

## 6. 编译运行
在 BasicVideoCall 或 Whiteboard 目录下，运行 `npm run dev`, 打开 http://localhost:8083/ (Whiteboard) 或 https://localhost:8082/ (BasicVideoCall) 即可使用。

## 7.关于独立白板 sdk

独立白板 sdk 代码示例请参考 Whiteboard/src/wbsdk.js 文件，您可以手动修改 Whiteboard/webpack.config.js 中 entry 字段，以 Whiteboard/src/wbsdk.js 为入口即可使用单独白板sdk