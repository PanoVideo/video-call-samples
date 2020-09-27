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
    contentBase: path.join(__dirname, './static'),
    hot: true,
    inline: false,
    compress: true,
    port: 8082,
    host: '0.0.0.0',
    https: true
  }
}
