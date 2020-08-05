# 拍乐云实时互动白板 Demo 使用指导

## 1. 注册拍乐云账号
进入拍乐云 [控制台](https://console.pano.video/) 页面，根据指导创建拍乐云账号。

## 2. 创建应用
登录拍乐云 [控制台](https://console.pano.video/) ，进入应用管理页面创建一个新的应用，获得 APPID，APPID 用以区分不同的实时音视频应用。

## 3. 下载 Demo 源码
拍乐云Demo源代码托管在 [github](https://github.com/PanoVideo) 上，用户可从github上clone。

## 4. 生成临时token
用户在创建应用获取到 APPID 后，还需要 token 才可以使用拍乐云实时音视频服务。对于 Demo 用户可在拍乐云控制台为应用生成临时token。

## 5. 安装依赖
在 web demo 目录下，运行 `npm i`

## 6. 编译运行
在 BasicVideoCall 目录下，运行 `npm run dev`, 打开 http://localhost:8083/ 即可使用。