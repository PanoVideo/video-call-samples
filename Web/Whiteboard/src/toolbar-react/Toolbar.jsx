import React from 'react'
import classnames from 'classnames'
import ColorPicker from 'rc-color-picker'
import 'rc-color-picker/assets/index.css'
import '../toolbar.scss'
import { get } from 'lodash-es'
import PanoRtc from '@pano.video/panortc'

const { Constants, RtcWhiteboard } = PanoRtc

/**
 *  0 透明 - 100 不透明
 * @param color rgba 颜色值
 */
function getAlphaOfRGBA(color) {
  const rgb = color.match(/(\d(\.\d+)?)+/g)
  return Number(get(rgb, 3, 0)) * 100
}

export default class Toolbar extends React.PureComponent {
  state = {
    clearPopupVisible: false,
    imgUrl: 'https://www.pano.video/assets/img/sdk/optimized_sdk4.jpg',
    isBackground: true,
    imgScaleMode: Constants.WBImageScalingMode.Fit,
    isAdmin: this.props.whiteboard.isAdmin,
    fontSize: this.props.whiteboard.fontSize,
    bold: this.props.whiteboard.bold,
    italic: this.props.whiteboard.italic,
    scale: this.props.whiteboard.scale,
    pageCount: this.props.whiteboard.getTotalNumberOfPages(),
    pageIndex: this.props.whiteboard.getCurrentPageNumber(),
    backgroundColor: this.props.whiteboard.backgroundColor,
    lineWidth: this.props.whiteboard.lineWidth,
    strokeStyle: this.props.whiteboard.strokeStyle,
    insertType: this.props.whiteboard.getToolType(),
    fillStyle: this.props.whiteboard.fillStyle,
    fillType: this.props.whiteboard.fillType,
    strokeStyleAlpha: getAlphaOfRGBA(this.props.whiteboard.strokeStyle),
    fillStyleAlpha: getAlphaOfRGBA(this.props.whiteboard.fillStyle),
    backgroundColorAlpha: getAlphaOfRGBA(this.props.whiteboard.backgroundColor),
    inputTypes: [
      { type: Constants.ShapeType.Select, icon: 'select', size: '12px' },
      { type: Constants.ShapeType.Pen, icon: 'pencil', size: '24px' },
      { type: Constants.ShapeType.Line, icon: 'line', size: '22px' },
      { type: Constants.ShapeType.Arrow, icon: 'arrow', size: '22px' },
      { type: Constants.ShapeType.Rect, icon: 'rect', size: '14px' },
      { type: Constants.ShapeType.Ellipse, icon: 'ellipse', size: '16px' },
      { type: Constants.ShapeType.Text, icon: 'text', size: '20px' },
      { type: Constants.ShapeType.Eraser, icon: 'eraser', size: '16px' },
      { type: Constants.ShapeType.Image, icon: 'image', size: '20px' },
      { type: Constants.ShapeType.Delete, icon: 'delete', size: '26px' }
    ]
  }

  componentDidMount() {
    this.props.whiteboard.on(
      RtcWhiteboard.Events.whiteboardContentUpdate,
      this.getStateFromProps
    )
    this.props.whiteboard.on(
      RtcWhiteboard.Events.userRoleTypeChanged,
      this.getStateFromProps
    )
  }

  componentWillUnmount() {
    this.props.whiteboard.off(
      RtcWhiteboard.Events.whiteboardContentUpdate,
      this.getStateFromProps
    )
    this.props.whiteboard.off(
      RtcWhiteboard.Events.userRoleTypeChanged,
      this.getStateFromProps
    )
  }

  getStateFromProps = () => {
    const backgroundColor = this.props.whiteboard.backgroundColor
    const strokeStyle = get(
      this.props.whiteboard.selectedShape,
      'shapeStyle.strokeStyle',
      this.props.whiteboard.strokeStyle
    )
    const fillStyle = get(
      this.props.whiteboard.selectedShape,
      'shapeStyle.fillStyle',
      this.props.whiteboard.fillStyle
    )
    const lineWidth = get(
      this.props.whiteboard.selectedShape,
      'shapeStyle.lineWidth',
      this.props.whiteboard.lineWidth
    )
    const bold = get(
      this.props.whiteboard.selectedShape,
      'fontStyle.bold',
      this.props.whiteboard.bold
    )
    const italic = get(
      this.props.whiteboard.selectedShape,
      'fontStyle.italic',
      this.props.whiteboard.italic
    )
    const fontSize = get(
      this.props.whiteboard.selectedShape,
      'fontStyle.fontSize',
      this.props.whiteboard.fontSize
    )
    const fillType = get(
      this.props.whiteboard.selectedShape,
      'shapeStyle.fillType',
      this.props.whiteboard.fillType
    )
    this.setState({
      fontSize,
      bold,
      italic,
      scale: this.props.whiteboard.scale,
      isAdmin: this.props.whiteboard.isAdmin,
      pageCount: this.props.whiteboard.getTotalNumberOfPages(),
      pageIndex: this.props.whiteboard.getCurrentPageNumber(),
      backgroundColor,
      lineWidth,
      strokeStyle,
      fillStyle,
      fillType,
      insertType: this.props.whiteboard.getToolType(),
      strokeStyleAlpha: getAlphaOfRGBA(strokeStyle),
      fillStyleAlpha: getAlphaOfRGBA(fillStyle),
      backgroundColorAlpha: getAlphaOfRGBA(backgroundColor)
    })
  }

  /**
   * 转 rgba 颜色
   * @param color 这种方式 #112233 表达的颜色
   * @param alpha 透明度
   */
  colorToRGBA(color, alpha) {
    const r = color.slice(1, 3)
    const g = color.slice(3, 5)
    const b = color.slice(5, 7)
    return `rgba(${parseInt(r, 16)},${parseInt(g, 16)},${parseInt(b, 16)},${
      alpha / 100
    })`
  }

  updatePageIndex(pageIndex) {
    if (
      pageIndex < 0 ||
      pageIndex >= this.props.whiteboard.getTotalNumberOfPages()
    )
      return
    this.props.whiteboard.gotoPage(pageIndex)
  }

  updateScale(nextScale) {
    this.props.whiteboard.scale = nextScale
  }

  toggleClearPopup = () => {
    this.setState({ clearPopupVisible: !this.state.clearPopupVisible })
  }

  renderToobar() {
    return (
      <div className="pano-whiteboard-toolbar pano-wb-tools">
        <div className="toolbar-input-types">
          {this.state.inputTypes.map((input) => (
            <div
              onClick={() => {
                this.props.whiteboard.setToolType(input.type)
                this.setState({ insertType: input.type })
              }}
              className={classnames({
                'input-type': true,
                'input-type--selected': this.state.insertType === input.type
              })}
              key={input.type}
            >
              <span
                className={`icon-${input.icon}`}
                style={{ fontSize: input.size }}
              ></span>
            </div>
          ))}

          <div className="input-type input-type--with-input">
            <label>
              Admin
              <input
                type="checkbox"
                checked={this.state.isAdmin}
                onChange={() => {
                  this.setState({ isAdmin: !this.state.isAdmin })
                  this.props.whiteboard.setRoleType(
                    this.props.whiteboard.isAdmin
                      ? Constants.WBRoleType.Attendee
                      : Constants.WBRoleType.Admin
                  )
                }}
              />
            </label>
          </div>
        </div>
        {this.renderImageInput()}
        {this.renderInputs()}
      </div>
    )
  }

  renderInputs() {
    return (
      <div className="toolbar-input-groups">
        <div className="toolbar-input-group">
          <div className="toolbar-input-group__label">颜色</div>
          <ColorPicker
            mode="RGB"
            animation="slide-up"
            color={this.state.strokeStyle}
            onChange={(data) => {
              const strokeStyle = this.colorToRGBA(data.color, data.alpha)
              if (this.props.whiteboard.selectedShape) {
                this.props.whiteboard.setSelectedShapeStyle({ strokeStyle })
              } else {
                this.props.whiteboard.strokeStyle = strokeStyle
              }
              this.setState({ strokeStyleAlpha: data.alpha, strokeStyle })
            }}
            placement="topLeft"
            alpha={this.state.strokeStyleAlpha}
          >
            <span className="rc-color-picker-trigger color-block" />
          </ColorPicker>
        </div>

        <div className="toolbar-input-group">
          <div className="toolbar-input-group__label">宽度</div>
          <input
            type="number"
            value={this.state.lineWidth}
            onChange={(e) => {
              const lineWidth = Number(e.target.value)
              if (lineWidth > 0) {
                if (this.props.whiteboard.selectedShape) {
                  this.props.whiteboard.setSelectedShapeStyle({ lineWidth })
                } else {
                  this.props.whiteboard.lineWidth = lineWidth
                }
                this.setState({ lineWidth })
              }
            }}
          />
        </div>

        <div className="toolbar-input-group">
          <div className="toolbar-input-group__label">填充</div>
          <input
            type="checkbox"
            checked={this.state.fillType === 'color'}
            onChange={() => {
              const fillType =
                this.state.fillType === 'color' ? 'none' : 'color'
              if (this.props.whiteboard.selectedShape) {
                this.props.whiteboard.setSelectedShapeStyle({ fillType })
              } else {
                this.props.whiteboard.fillType = fillType
              }
              this.setState({
                fillType: fillType
              })
            }}
          />
          <ColorPicker
            animation="slide-up"
            color={this.state.fillStyle}
            onChange={(data) => {
              const fillStyle = this.colorToRGBA(data.color, data.alpha)
              if (this.props.whiteboard.selectedShape) {
                this.props.whiteboard.setSelectedShapeStyle({ fillStyle })
              } else {
                this.props.whiteboard.fillStyle = fillStyle
              }
              this.setState({
                fillStyleAlpha: data.alpha,
                fillStyle
              })
            }}
            placement="topLeft"
            alpha={this.state.fillStyleAlpha}
          >
            <span className="rc-color-picker-trigger color-block" />
          </ColorPicker>
        </div>

        <div className="toolbar-input-group">
          <div className="toolbar-input-group__label">背景</div>
          <ColorPicker
            animation="slide-up"
            color={this.state.backgroundColor}
            onChange={(data) => {
              const backgroundColor = this.colorToRGBA(data.color, data.alpha)
              this.setState({
                backgroundColorAlpha: data.alpha,
                backgroundColor
              })
              this.props.whiteboard.backgroundColor = backgroundColor
            }}
            placement="topLeft"
            alpha={this.state.backgroundColorAlpha}
          >
            <span className="rc-color-picker-trigger color-block" />
          </ColorPicker>
        </div>

        <div className="toolbar-input-group">
          <div className="label">字体</div>
          <input
            type="number"
            value={this.state.fontSize}
            onChange={(e) => {
              const fontSize = Number(e.target.value)
              if (fontSize > 0) {
                if (this.props.whiteboard.selectedShape) {
                  this.props.whiteboard.setSelectedShapeFontStyle({
                    fontSize
                  })
                } else {
                  this.props.whiteboard.fontSize = fontSize
                }
                this.setState({ fontSize })
              }
            }}
          />
        </div>

        <div className="toolbar-input-group">
          <div
            onClick={() => {
              if (this.props.whiteboard.selectedShape) {
                this.props.whiteboard.setSelectedShapeFontStyle({
                  bold: !this.state.bold
                })
              } else {
                this.props.whiteboard.bold = !this.state.bold
              }
              this.setState({ bold: !this.state.bold })
            }}
            className={classnames({
              'toolbar-input-group__svg': true,
              'toolbar-input-group__svg--selected': this.state.bold
            })}
          >
            <span className="icon-bold" style={{ fontSize: '15px' }}></span>
          </div>
        </div>

        <div className="toolbar-input-group">
          <div
            onClick={() => {
              if (this.props.whiteboard.selectedShape) {
                this.props.whiteboard.setSelectedShapeFontStyle({
                  italic: !this.state.italic
                })
              } else {
                this.props.whiteboard.italic = !this.state.italic
              }
              this.setState({ italic: !this.state.italic })
            }}
            className={classnames({
              'toolbar-input-group__svg': true,
              'toolbar-input-group__svg--selected': this.state.italic
            })}
          >
            <span className="icon-italic"></span>
          </div>
        </div>
      </div>
    )
  }

  renderImageInput() {
    return (
      <>
        {this.state.insertType === Constants.ShapeType.Image && (
          <div className="wb-img-wrapper">
            <div className="wb-img-wrapper__label">Img Url</div>
            <input
              type="text"
              value={this.state.imgUrl}
              onChange={(e) => {
                this.setState({ imgUrl: e.target.value })
              }}
            />

            {/* <div className="wb-img-wrapper__label">Background</div>
            <input
              type="checkbox"
              checked={this.state.isBackground}
              onChange={(e) =>
                this.setState({ isBackground: e.target.checked })
              }
            /> */}

            <div className="wb-img-wrapper__label">Scale Mode</div>
            <label>
              <input
                type="radio"
                name="fillMode"
                value={Constants.WBImageScalingMode.Fit}
                checked={
                  this.state.imgScaleMode === Constants.WBImageScalingMode.Fit
                }
                onChange={() => {
                  this.setState({
                    imgScaleMode: Constants.WBImageScalingMode.Fit
                  })
                  this.props.whiteboard.setBackgroundImageScalingMode(
                    Constants.WBImageScalingMode.Fit
                  )
                }}
              />
              Fit
            </label>
            <label>
              <input
                type="radio"
                name="fillMode"
                value={Constants.WBImageScalingMode.FillWidth}
                checked={
                  this.state.imgScaleMode ===
                  Constants.WBImageScalingMode.FillWidth
                }
                onChange={() => {
                  this.setState({
                    imgScaleMode: Constants.WBImageScalingMode.FillWidth
                  })
                  this.props.whiteboard.setBackgroundImageScalingMode(
                    Constants.WBImageScalingMode.FillWidth
                  )
                }}
              />
              Fill width
            </label>
            <label>
              <input
                type="radio"
                name="fillMode"
                value={Constants.WBImageScalingMode.FillHeight}
                checked={
                  this.state.imgScaleMode ===
                  Constants.WBImageScalingMode.FillHeight
                }
                onChange={() => {
                  this.setState({
                    imgScaleMode: Constants.WBImageScalingMode.FillHeight
                  })
                  this.props.whiteboard.setBackgroundImageScalingMode(
                    Constants.WBImageScalingMode.FillHeight
                  )
                }}
              />
              Fill height
            </label>
            <button
              className="wb-img-wrapper__confirm"
              onClick={() => {
                this.props.whiteboard.uploadImage((err) => {
                  if (err) {
                    alert('上传失败:' + err.message)
                  }
                })
              }}
            >
              Upload
            </button>
            <button
              className="wb-img-wrapper__confirm"
              onClick={() => {
                if (!this.state.imgUrl) {
                  return
                }
                this.props.whiteboard.setBackgroundImage(this.state.imgUrl)
              }}
            >
              Add
            </button>
          </div>
        )}
      </>
    )
  }

  renderPagination() {
    return (
      <div className="pano-wb-pages pano-wb-tools">
        <button
          className={classnames({
            'pano-wb-pages__btn': true
          })}
          onClick={() => this.updatePageIndex(this.state.pageIndex - 1)}
        >
          <span className="icon-left"></span>
        </button>
        <div className="pano-wb-pages__page-count">
          {`${this.state.pageIndex + 1} / ${this.state.pageCount}`}
        </div>

        <button
          className={classnames({
            'pano-wb-pages__btn': true
          })}
          onClick={() => this.updatePageIndex(this.state.pageIndex + 1)}
        >
          <span className="icon-right"></span>
        </button>

        <button
          className={classnames({
            'pano-wb-pages__btn-add': true
          })}
          onClick={() => {
            this.props.whiteboard.addPage()
          }}
        >
          <span className="icon-add"></span>
        </button>

        <button
          className={classnames({
            'pano-wb-pages__btn-add': true
          })}
          onClick={() => this.props.whiteboard.removePage()}
        >
          <span className="icon-minus"></span>
        </button>
      </div>
    )
  }

  renderZoomTool() {
    return (
      <div className="pano-wb-zoom pano-wb-tools">
        <div
          className="pano-wb-zoom__btn"
          onClick={() =>
            this.updateScale(
              this.state.scale > 0.15 ? this.state.scale - 0.05 : 0.1
            )
          }
        >
          <span className="icon-minus"></span>
        </div>

        <div className="pano-wb-zoom__zoom-rate">
          {Math.round(this.state.scale * 100)}%
        </div>

        <div
          className="pano-wb-zoom__btn"
          onClick={() => {
            this.updateScale(
              this.state.scale < 1.95 ? this.state.scale + 0.05 : 2
            )
          }}
        >
          <span className="icon-add"></span>
        </div>

        <div
          className="pano-wb-zoom__btn"
          onClick={() => {
            this.updateScale(1)
            this.props.whiteboard.translate[0] = 0
            this.props.whiteboard.translate[1] = 0
          }}
        >
          <span className="icon-revert"></span>
        </div>

        <div
          onClick={() => this.props.whiteboard.undo()}
          className="pano-wb-zoom__btn"
        >
          <span className="icon-undo"></span>
        </div>

        <div
          onClick={() => this.props.whiteboard.redo()}
          className="pano-wb-zoom__btn"
        >
          <span className="icon-redo"></span>
        </div>

        <div onClick={this.toggleClearPopup} className="pano-wb-zoom__btn">
          <span className="icon-delete" style={{ fontSize: '26px' }}></span>
        </div>

        {this.state.clearPopupVisible && (
          <div className="pano-wb-zoom__clear-popup">
            <button
              disabled={!this.state.isAdmin}
              className="wb-clear-popup-item"
              onClick={() => {
                this.props.whiteboard.clearContents(
                  false,
                  Constants.WBClearType.All
                )
                this.toggleClearPopup()
              }}
            >
              Clear All
            </button>
            <button
              disabled={!this.state.isAdmin}
              className="wb-clear-popup-item"
              onClick={() => {
                this.props.whiteboard.clearContents(
                  true,
                  Constants.WBClearType.All
                )
                this.toggleClearPopup()
              }}
            >
              Clear Current Page
            </button>
            <button
              className="wb-clear-popup-item"
              onClick={() => {
                this.props.whiteboard.clearContents(
                  false,
                  Constants.WBClearType.DRAWS
                )
                this.toggleClearPopup()
              }}
            >
              Clear My Contents
            </button>
            <button
              className="wb-clear-popup-item"
              onClick={() => {
                this.props.whiteboard.clearContents(
                  false,
                  Constants.WBClearType.BACKGROUND_IMAGE
                )
                this.toggleClearPopup()
              }}
            >
              Clear My Background
            </button>
          </div>
        )}
      </div>
    )
  }

  render() {
    return (
      <>
        {this.renderToobar()}
        {this.renderPagination()}
        {this.renderZoomTool()}
      </>
    )
  }
}
