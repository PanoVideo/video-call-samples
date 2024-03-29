# 拍乐云实时音视频 Demo 使用指导

## 1. 注册拍乐云账号
访问拍乐云 [注册账号](https://console.pano.video/#/user/register) 页面，根据提示创建拍乐云账号。

## 2. 创建应用
登录拍乐云 [控制台](https://console.pano.video) ，进入应用管理页面创建一个新的应用，获得 AppID (用以区分不同的实时音视频应用)。

## 3. 下载 Demo 源码
此 Demo 源代码托管在 [GitHub](https://github.com/PanoVideo) 和 [Gitee](https://gitee.com/pano-video) 上，开发者可以自行克隆或下载。

## 4. 下载 SDK
拍乐云实时音视频 SDK 可从拍乐云 [下载页面](https://www.pano.video/download.html#sdk) 下载，并放置到 Demo 源码的 SDK 目录中。

## 5. 生成临时 Token
用户在创建应用并且获取到 AppID 后，还需要 Token 才可以使用拍乐云实时音视频服务。对于 Demo 用户可在拍乐云控制台为应用 <a href="https://developer.pano.video/getting-started/firstapp/#14-%E7%94%9F%E6%88%90%E4%B8%B4%E6%97%B6token">生成临时Token</a>。

## 6. 编译 Demo 源码
使用 Xcode（11.0 或以上版本）打开源码工程 BasicVideoCall，然后将 AppID 和 Token 拷贝到 Demo 中相应位置，即可开始编译运行。

## 7. 开发环境要求
- 兼容 iOS 9.0 及以上版本
- Xcode 11.0 或以上版本