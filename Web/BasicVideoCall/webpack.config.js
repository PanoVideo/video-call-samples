const path = require('path')

module.exports = {
  devtool: 'eval-source-map',
  mode: 'development',
  entry: path.resolve(__dirname, './src/index.js'),
  output: {
    path: path.resolve(__dirname, './index'),
    filename: 'index.js'
  },
  externals: {
    fs: 'fs',
    ws: 'ws'
  },
  devServer: {
    static: path.join(__dirname, './static'),
    hot: true,
    compress: true,
    port: 8082,
    host: '127.0.0.1',
    https: true
  }
}
