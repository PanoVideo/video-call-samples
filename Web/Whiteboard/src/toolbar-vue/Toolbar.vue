<template>
  <div>
    <div class="pano-whiteboard-toolbar pano-wb-tools">
      <div class="toolbar-input-types">
        <div
          v-for="input in inputTypes"
          @click="setToolType(input)"
          :class="{
            'input-type': true,
            'input-type--selected': insertType === input.type
          }"
          :key="input.type"
        >
          <span
            :class="`icon-${input.icon}`"
            :style="{ fontSize: input.size }"
          ></span>
        </div>

        <!-- <div @click="uploadDoc" class="input-type">
          <span class="icon-upload" style="font-size: 20px"></span>
        </div> -->

        <div class="input-type input-type--with-input">
          <label>
            Admin
            <input type="checkbox" :checked="isAdmin" @input="setAdmin" />
          </label>
        </div>
      </div>

      <div
        class="wb-img-wrapper"
        v-if="insertType === Constants.ShapeType.Image"
      >
        <div class="wb-img-wrapper__label">Img Url</div>
        <input type="text" v-model="imgUrl" />

        <!-- <div class="wb-img-wrapper__label">Background</div>
        <input type="checkbox" v-model="background" /> -->

        <div class="wb-img-wrapper__label">Scale Mode</div>
        <label>
          <input
            type="radio"
            name="fillMode"
            :value="Constants.WBImageScalingMode.Fit"
            v-model="imgScaleMode"
          />
          Fit
        </label>
        <label>
          <input
            type="radio"
            name="fillMode"
            :value="Constants.WBImageScalingMode.FillWidth"
            v-model="imgScaleMode"
          />
          Fill width
        </label>
        <label>
          <input
            type="radio"
            name="fillMode"
            :value="Constants.WBImageScalingMode.FillHeight"
            v-model="imgScaleMode"
          />
          Fill height
        </label>
        <button class="wb-img-wrapper__confirm" @click="uploadImage">
          Upload
        </button>
        <button class="wb-img-wrapper__confirm" @click="setImageBg">
          Add
        </button>
      </div>

      <div class="wb-img-wrapper">
        <div class="wb-img-wrapper__label">Img Url List</div>
        <input
          class="wb-img-wrapper__urls-input"
          type="text"
          v-model="imgUrlList"
          placeholder="url以空格隔开"
        />

        <div class="wb-img-wrapper__label">Add to current page</div>
        <input type="checkbox" v-model="bAddToCurrentDoc" />
        <button class="wb-img-wrapper__confirm" @click="addImagBgList">
          Add
        </button>
      </div>

      <!-- 输入框 -->
      <div class="toolbar-input-groups">
        <div class="toolbar-input-group">
          <div class="toolbar-input-group__label">颜色</div>

          <ColorPicker
            show-alpha
            size="mini"
            :predefine="predefineColors"
            v-model="strokeStyle"
          />
        </div>

        <div class="toolbar-input-group">
          <div class="toolbar-input-group__label">宽度</div>
          <input type="number" v-model="lineWidth" />
        </div>

        <div class="toolbar-input-group">
          <div class="toolbar-input-group__label">填充</div>
          <input
            type="checkbox"
            :checked="fillType === 'color'"
            @change="changeFillType"
          />
          <ColorPicker
            show-alpha
            size="mini"
            :predefine="predefineColors"
            v-model="fillStyle"
          />
        </div>

        <div class="toolbar-input-group">
          <div class="toolbar-input-group__label">背景</div>

          <ColorPicker
            show-alpha
            size="mini"
            :predefine="predefineColors"
            v-model="backgroundColor"
          />
        </div>

        <div class="toolbar-input-group">
          <div class="label">字体</div>
          <input type="number" v-model="fontSize" />
        </div>

        <div class="toolbar-input-group">
          <div
            @click="toggleBold"
            :class="{
              'toolbar-input-group__svg': true,
              'toolbar-input-group__svg--selected': bold
            }"
          >
            <span class="icon-bold" style="font-size: 15px;"></span>
          </div>
        </div>

        <div class="toolbar-input-group">
          <div
            @click="toggleItalic"
            :class="{
              'toolbar-input-group__svg': true,
              'toolbar-input-group__svg--selected': italic
            }"
          >
            <span class="icon-italic"></span>
          </div>
        </div>
      </div>
    </div>

    <!-- pagination -->
    <div class="pano-wb-pages pano-wb-tools">
      <div class="pano-wb-pages__delete-doc-wrapper">
        <span @click="deleteDoc" class="pano-wb-pages__delete-doc">
          <span class="icon-delete" style="font-size: 26px"></span>
        </span>
        <select name="doc" :value="whiteboard.activeDocId" @change="switchDoc">
          <option
            v-for="(doc, index) in docs"
            :key="index"
            :value="doc.docId"
            >{{ doc.name || doc.docId }}</option
          >
        </select>
      </div>
      <button
        :class="{
          'pano-wb-pages__btn': true
        }"
        @click="goPrevPage"
      >
        <span class="icon-left"></span>
      </button>
      <div class="pano-wb-pages__page-count">
        {{ `${pageIndex + 1} / ${pageCount}` }}
      </div>

      <button
        :class="{
          'pano-wb-pages__btn': true
        }"
        @click="goNextPage"
      >
        <span class="icon-right"></span>
      </button>

      <button
        :class="{
          'pano-wb-pages__btn-add': true
        }"
        @click="addPage"
      >
        <span class="icon-add"></span>
      </button>

      <button
        :class="{
          'pano-wb-pages__btn-add': true
        }"
        @click="removePage"
      >
        <span class="icon-minus"></span>
      </button>
    </div>

    <!-- zoom -->
    <div class="pano-wb-zoom pano-wb-tools">
      <div
        class="pano-wb-zoom__btn"
        @click="updateScale(scale > 0.15 ? scale - 0.05 : 0.1)"
      >
        <span class="icon-minus"></span>
      </div>

      <div class="pano-wb-zoom__zoom-rate">{{ Math.round(scale * 100) }}%</div>

      <div
        class="pano-wb-zoom__btn"
        @click="updateScale(scale < 1.95 ? scale + 0.05 : 2)"
      >
        <span class="icon-add"></span>
      </div>

      <div class="pano-wb-zoom__btn" @click="reset">
        <span class="icon-revert"></span>
      </div>

      <div @click="undo" class="pano-wb-zoom__btn">
        <span class="icon-undo"></span>
      </div>

      <div @click="redo" class="pano-wb-zoom__btn">
        <span class="icon-redo"></span>
      </div>

      <div @click="toggleClearPopup" class="pano-wb-zoom__btn">
        <span class="icon-delete" style="font-size: 26px;"></span>
      </div>

      <div class="pano-wb-zoom__clear-popup" v-if="clearPopupVisible">
        <button
          :disabled="!isAdmin"
          class="wb-clear-popup-item"
          @click="clearAll"
        >
          Clear All
        </button>
        <button
          :disabled="!isAdmin"
          class="wb-clear-popup-item"
          @click="clearCurrentPage"
        >
          Clear Current Page
        </button>
        <button class="wb-clear-popup-item" @click="clearMyContent">
          Clear My Contents
        </button>
        <button class="wb-clear-popup-item" @click="clearMyBackground">
          Clear My Background
        </button>
      </div>
    </div>
  </div>
</template>

<script>
import { get } from 'lodash-es'
import PanoRtc from '@pano.video/panortc'
import { ColorPicker } from 'element-ui'
import 'element-ui/lib/theme-chalk/index.css'
import '../icons/style.css'

const { Constants, RtcWhiteboard } = PanoRtc
export default {
  data() {
    return {
      Constants,
      clearPopupVisible: false,
      imgUrl: 'https://www.pano.video/assets/img/sdk/optimized_sdk4.jpg',
      isBackground: true,
      imgScaleMode: Constants.WBImageScalingMode.Fit,
      isAdmin: this.whiteboard.isAdmin,
      fontSize: this.whiteboard.fontSize,
      bold: this.whiteboard.bold,
      italic: this.whiteboard.italic,
      scale: this.whiteboard.scale,
      pageCount: this.whiteboard.getTotalNumberOfPages(),
      pageIndex: this.whiteboard.getCurrentPageNumber(),
      backgroundColor: this.whiteboard.backgroundColor,
      lineWidth: this.whiteboard.lineWidth,
      strokeStyle: this.whiteboard.strokeStyle,
      insertType: this.whiteboard.getToolType(),
      fillStyle: this.whiteboard.fillStyle,
      fillType: this.whiteboard.fillType,
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
      ],
      predefineColors: [
        '#ff4500',
        '#ff8c00',
        '#ffd700',
        '#90ee90',
        '#00ced1',
        '#1e90ff',
        '#c71585',
        'rgba(255, 69, 0, 0.68)',
        'rgb(255, 120, 0)',
        'hsv(51, 100, 98)',
        'hsva(120, 40, 94, 0.5)',
        'hsl(181, 100%, 37%)',
        'hsla(209, 100%, 56%, 0.73)',
        '#c7158577'
      ],
      docs: this.whiteboard.enumerateDocs(),
      imgUrlList:
        'http://t7.baidu.com/it/u=378254553,3884800361&fm=79&app=86&f=JPEG?w=1280&h=2030 http://t8.baidu.com/it/u=3571592872,3353494284&fm=79&app=86&f=JPEG?w=1200&h=1290 http://t7.baidu.com/it/u=3616242789,1098670747&fm=79&app=86&f=JPEG?w=900&h=1350',
      bAddToCurrentDoc: false
    }
  },
  components: {
    ColorPicker
  },
  props: {
    whiteboard: Object
  },
  watch: {
    fontSize(newValue) {
      if (newValue > 0) {
        if (this.whiteboard.selectedShape) {
          this.whiteboard.setSelectedShapeFontStyle({
            fontSize: newValue
          })
        } else {
          this.whiteboard.fontSize = newValue
        }
      } else {
        this.fontSize = 1
      }
    },
    fillStyle(newValue) {
      const fillStyle = newValue || 'rgba(112, 124, 138, 1)'
      if (this.whiteboard.selectedShape) {
        this.whiteboard.setSelectedShapeStyle({ fillStyle })
      } else {
        this.whiteboard.fillStyle = fillStyle
      }
      this.fillStyle = fillStyle
    },
    strokeStyle(newValue) {
      const strokeStyle = newValue || 'rgba(33, 212, 183, 1)'
      if (this.whiteboard.selectedShape) {
        this.whiteboard.setSelectedShapeStyle({ strokeStyle })
      } else {
        this.whiteboard.strokeStyle = strokeStyle
      }
      this.strokeStyle = strokeStyle
    },
    backgroundColor(newValue) {
      const backgroundColor = newValue || 'rgba(254,255,255,1)'
      this.whiteboard.backgroundColor = backgroundColor
      this.backgroundColor = backgroundColor
    },
    imgScaleMode(newValue) {
      this.whiteboard.setBackgroundImageScalingMode(newValue)
    },
    lineWidth(newValue) {
      const lineWidth = newValue
      if (lineWidth > 0) {
        if (this.whiteboard.selectedShape) {
          this.whiteboard.setSelectedShapeStyle({ lineWidth })
        } else {
          this.whiteboard.lineWidth = lineWidth
        }
      } else {
        this.lineWidth = 1
      }
    }
  },
  methods: {
    uploadDoc() {
      this.whiteboard.uploadDoc(console.log)
    },
    onDocChanged() {
      this.docs = this.whiteboard.enumerateDocs()
    },
    switchDoc(e) {
      this.whiteboard.switchToDoc(e.target.value)
    },
    deleteDoc() {
      this.whiteboard.deleteDoc(this.whiteboard.activeDocId)
    },
    addImagBgList() {
      this.whiteboard.addBackgroundImages(
        this.imgUrlList.split(' '),
        this.bAddToCurrentDoc
      )
    },
    removePage() {
      this.whiteboard.removePage()
    },
    addPage() {
      this.whiteboard.addPage()
    },
    undo() {
      this.whiteboard.undo()
    },
    redo() {
      this.whiteboard.redo()
    },
    clearMyBackground() {
      console.log('clearMyBackground')
      this.whiteboard.clearContents(
        false,
        Constants.WBClearType.BACKGROUND_IMAGE
      )
      this.toggleClearPopup()
    },
    clearMyContent() {
      console.log('clearMyContent')
      this.whiteboard.clearContents(false, Constants.WBClearType.DRAWS)
      this.toggleClearPopup()
    },
    clearCurrentPage() {
      console.log('clearCurrentPage')
      this.whiteboard.clearContents(true, Constants.WBClearType.All)
      this.toggleClearPopup()
    },
    clearAll() {
      console.log('clearAll')
      this.whiteboard.clearContents(false, Constants.WBClearType.All)
      this.toggleClearPopup()
    },
    reset() {
      this.updateScale(1)
      this.whiteboard.translate[0] = 0
      this.whiteboard.translate[1] = 0
    },
    goNextPage() {
      this.updatePageIndex(this.pageIndex + 1)
    },
    goPrevPage() {
      this.updatePageIndex(this.pageIndex - 1)
    },
    toggleItalic() {
      if (this.whiteboard.selectedShape) {
        this.whiteboard.setSelectedShapeFontStyle({
          italic: !this.italic
        })
      } else {
        this.whiteboard.italic = !this.italic
      }
      this.italic = !this.italic
    },
    toggleBold() {
      if (this.whiteboard.selectedShape) {
        this.whiteboard.setSelectedShapeFontStyle({
          bold: !this.bold
        })
      } else {
        this.whiteboard.bold = !this.bold
      }
      this.bold = !this.bold
    },
    changeFillType() {
      const fillType = this.fillType === 'color' ? 'none' : 'color'
      if (this.whiteboard.selectedShape) {
        this.whiteboard.setSelectedShapeStyle({ fillType })
      } else {
        this.whiteboard.fillType = fillType
      }
      this.fillType = fillType
    },
    setImageBg() {
      if (!this.imgUrl) {
        return
      }
      this.whiteboard.setBackgroundImage(this.imgUrl)
    },
    uploadImage() {
      this.whiteboard.uploadImage((err) => {
        if (err) {
          alert('上传失败:' + err.message)
        }
      })
    },
    setAdmin() {
      this.isAdmin = !this.isAdmin
      this.whiteboard.setRoleType(
        this.isAdmin
          ? Constants.WBRoleType.Admin
          : Constants.WBRoleType.Attendee
      )
    },
    setToolType(input) {
      this.whiteboard.setToolType(input.type)
      this.insertType = input.type
    },
    updateScale(nextScale) {
      this.whiteboard.scale = nextScale
    },
    toggleClearPopup() {
      this.clearPopupVisible = !this.clearPopupVisible
    },
    updatePageIndex(pageIndex) {
      if (pageIndex < 0 || pageIndex >= this.whiteboard.getTotalNumberOfPages())
        return
      this.whiteboard.gotoPage(pageIndex)
    },
    getStateFromProps() {
      this.backgroundColor = this.whiteboard.backgroundColor
      this.strokeStyle = get(
        this.whiteboard.selectedShape,
        'shapeStyle.strokeStyle',
        this.whiteboard.strokeStyle
      )
      this.fillStyle = get(
        this.whiteboard.selectedShape,
        'shapeStyle.fillStyle',
        this.whiteboard.fillStyle
      )
      this.lineWidth = get(
        this.whiteboard.selectedShape,
        'shapeStyle.lineWidth',
        this.whiteboard.lineWidth
      )
      this.bold = get(
        this.whiteboard.selectedShape,
        'fontStyle.bold',
        this.whiteboard.bold
      )
      this.italic = get(
        this.whiteboard.selectedShape,
        'fontStyle.italic',
        this.whiteboard.italic
      )
      this.fontSize = get(
        this.whiteboard.selectedShape,
        'fontStyle.fontSize',
        this.whiteboard.fontSize
      )
      this.fillType = get(
        this.whiteboard.selectedShape,
        'shapeStyle.fillType',
        this.whiteboard.fillType
      )
      this.scale = this.whiteboard.scale
      this.isAdmin = this.whiteboard.isAdmin
      this.pageCount = this.whiteboard.getTotalNumberOfPages()
      this.pageIndex = this.whiteboard.getCurrentPageNumber()
      this.insertType = this.whiteboard.getToolType()
    }
  },
  mounted() {
    this.whiteboard.on(
      RtcWhiteboard.Events.whiteboardContentUpdate,
      this.getStateFromProps
    )
    this.whiteboard.on(
      RtcWhiteboard.Events.userRoleTypeChanged,
      this.getStateFromProps
    )
    this.whiteboard.on(RtcWhiteboard.Events.docCreated, this.onDocChanged)
    this.whiteboard.on(RtcWhiteboard.Events.docSwitched, this.onDocChanged)
    this.whiteboard.on(RtcWhiteboard.Events.docDeleted, this.onDocChanged)
  },
  beforeDestroy() {
    this.whiteboard.off(
      RtcWhiteboard.Events.whiteboardContentUpdate,
      this.getStateFromProps
    )
    this.whiteboard.off(
      RtcWhiteboard.Events.userRoleTypeChanged,
      this.getStateFromProps
    )
    this.whiteboard.off(RtcWhiteboard.Events.docCreated, this.onDocChanged)
    this.whiteboard.off(RtcWhiteboard.Events.docSwitched, this.onDocChanged)
    this.whiteboard.off(RtcWhiteboard.Events.docDeleted, this.onDocChanged)
  }
}
</script>

<style src="../toolbar.scss"></style>

<style lang="scss">
.el-color-picker--mini .el-color-picker__trigger {
  height: 24px;
  width: 24px;
  top: 3px;
  left: 5px;
}
</style>
