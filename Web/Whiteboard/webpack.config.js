const path = require('path')
const VueLoaderPlugin = require('vue-loader/lib/plugin')

module.exports = {
  devtool: 'eval-source-map',
  mode: 'development',
  plugins: [new VueLoaderPlugin()],
  module: {
    rules: [
      {
        test: /\.(eot|woff2|woff|ttf|svg)/,
        use: [
          {
            loader: 'url-loader',
            options: {
              name: '[name][hash:5].min.[ext]',
              limit: 5000,
              publicPath: '',
              outputPath: 'dist/',
              useRelativePath: true
            }
          }
        ]
      },
      {
        test: /\.vue$/,
        use: {
          loader: 'vue-loader'
        }
      },
      {
        test: /\.js|jsx$/,
        use: 'babel-loader',
        exclude: /node_modules/
      },
      {
        test: /\.css/,
        use: ['style-loader', 'css-loader']
      },
      {
        test: /\.scss/,
        use: [
          'style-loader',
          'css-loader',
          {
            loader: 'sass-loader',
            options: {
              implementation: require('sass')
            }
          }
        ],
        exclude: /node_modules/
      }
    ]
  },
  entry: path.resolve(__dirname, './src/index.js'),
  // entry: path.resolve(__dirname, './src/wbsdk.js'),
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
    port: 8083,
    host: '127.0.0.1'
  }
}
