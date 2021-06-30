<template>
  <div class="pano-wb-tb-wrapper">
    <!-- 工具条 -->
    <div class="pano-wb-tb">
      <!-- 选择 -->
      <div
        :class="{
          'pano-wb-tb__item': true,
          'pano-withtip': true,
          'pano-wb-tb__item--selected':
            insertType === Constants.ShapeType.Select
        }"
        @click="setToolType(Constants.ShapeType.Select)"
        class="pano-wb-tb__item pano-withtip "
        data-tip="选择"
      >
        <span class="iconfont icon-move" />
      </div>

      <!-- 选择 -->
      <div
        :class="{
          'pano-wb-tb__item': true,
          'pano-withtip': true,
          'pano-wb-tb__item--selected':
            insertType === Constants.ShapeType.Click
        }"
        @click="setToolType(Constants.ShapeType.Click)"
        class="pano-wb-tb__item pano-withtip "
        data-tip="课件交互"
      >
        <span class="iconfont icon-click" />
      </div>

      <!-- 画笔 -->
      <div
        :class="{
          'pano-withtip': true,
          'pano-wb-tb__item': true,
          'pano-wb-tb__item--selected': insertType === Constants.ShapeType.Pen
        }"
        @click="setToolType(Constants.ShapeType.Pen)"
        data-tip="画笔"
      >
        <span class="iconfont icon-pencil" />
      </div>

      <!-- 空心图形 -->
      <Popover placement="right" trigger="click">
        <div class="pano-wb-popup">
          <div class="pano-wb-popup__item">
            <div
              :class="{
                'pano-wb-popup__item__select': true,
                'pano-wb-popup__item__select--selected':
                  insertType === Constants.ShapeType.Line
              }"
              @click="setToolType(Constants.ShapeType.Line)"
            >
              <span class="iconfont icon-minus" />
            </div>
            <div
              :class="{
                'pano-wb-popup__item__select': true,
                'pano-wb-popup__item__select--selected':
                  insertType === Constants.ShapeType.Arrow
              }"
              @click="setToolType(Constants.ShapeType.Arrow)"
            >
              <span class="iconfont icon-arrow-left2" />
            </div>
            <div
              :class="{
                'pano-wb-popup__item__select': true,
                'pano-wb-popup__item__select--selected':
                  fillType === 'none' &&
                  insertType === Constants.ShapeType.Ellipse
              }"
              @click="setToolType(Constants.ShapeType.Ellipse)"
            >
              <span class="iconfont icon-radio-unchecked" />
            </div>
            <div
              :class="{
                'pano-wb-popup__item__select': true,
                'pano-wb-popup__item__select--selected':
                  fillType === 'none' && insertType === Constants.ShapeType.Rect
              }"
              @click="setToolType(Constants.ShapeType.Rect)"
            >
              <span class="iconfont icon-checkbox-unchecked" />
            </div>
          </div>
        </div>
        <div
          :class="{
            'pano-withtip': true,
            'pano-wb-tb__item': true,
            'pano-wb-tb__item--selected':
              fillType === 'none' &&
              (insertType === Constants.ShapeType.Line ||
                insertType === Constants.ShapeType.Arrow ||
                insertType === Constants.ShapeType.Rect ||
                insertType === Constants.ShapeType.Ellipse)
          }"
          data-tip="空心图形"
          slot="reference"
        >
          <span class="iconfont icon-hollow" />
          <div class="pano-wb-tb__item__triangle" />
        </div>
      </Popover>

      <!-- 实心图形 -->
      <Popover placement="right" trigger="click">
        <div
          :class="{
            'pano-withtip': true,
            'pano-wb-tb__item': true,
            'pano-wb-tb__item--selected':
              fillType === 'color' &&
              (insertType === Constants.ShapeType.Rect ||
                insertType === Constants.ShapeType.Ellipse)
          }"
          data-tip="实心图形"
          slot="reference"
        >
          <span class="iconfont icon-solid" />
          <div class="pano-wb-tb__item__triangle" />
        </div>
        <div class="pano-wb-popup">
          <div class="pano-wb-popup__item">
            <div
              :class="{
                'pano-wb-popup__item__select': true,
                'pano-wb-popup__item__select--selected':
                  fillType === 'color' &&
                  insertType === Constants.ShapeType.Ellipse
              }"
              @click="setSolidToolType(Constants.ShapeType.Ellipse)"
            >
              <span class="iconfont icon-circle-h" />
            </div>
            <div
              :class="{
                'pano-wb-popup__item__select': true,
                'pano-wb-popup__item__select--selected':
                  fillType === 'color' &&
                  insertType === Constants.ShapeType.Rect
              }"
              @click="setSolidToolType(Constants.ShapeType.Rect)"
            >
              <span class="iconfont icon-rect-h" />
            </div>
          </div>
        </div>
      </Popover>

      <!-- 文本 -->
      <Popover placement="right" trigger="click">
        <div
          :class="{
            'pano-wb-tb__item': true,
            'pano-withtip': true,
            'pano-wb-tb__item--selected':
              insertType === Constants.ShapeType.Text
          }"
          slot="reference"
          data-tip="文本"
          @click="setToolType(Constants.ShapeType.Text)"
        >
          <span class="iconfont icon-text" />
          <div class="pano-wb-tb__item__triangle" />
        </div>
        <div class="pano-wb-popup">
          <div
            :class="{
              'pano-wb-popup__item__select': true,
              'pano-wb-popup__item__select--selected': bold
            }"
            @click="toggleBold"
          >
            <span class="iconfont icon-bold" />
          </div>
          <div
            :class="{
              'pano-wb-popup__item__select': true,
              'pano-wb-popup__item__select--selected': italic
            }"
            @click="toggleItalic"
          >
            <span class="iconfont icon-italic" />
          </div>
        </div>
      </Popover>

      <!-- 工具箱 -->
      <Popover placement="right" trigger="click">
        <div class="pano-wb-popup">
          <div class="pano-wb-popup__item">
            <span class="pano-wb-popup__item__label">线宽：</span>
            <el-slider
              :value="lineWidth"
              @input="setLineWidth"
              :step="1"
              :min="1"
              :max="20"
              style="flex: 1; max-width: 140px"
            />
          </div>
          <div class="pano-wb-popup__item">
            <span class="pano-wb-popup__item__label">字号：</span>
            <el-slider
              :step="2"
              :min="12"
              :max="60"
              :value="fontSize"
              @input="setFontSize"
              style="flex: 1; max-width: 140px"
            />
          </div>
          <div class="pano-wb-popup__item">
            <div
              v-for="color in colors"
              class="pano-wb-popup__item__colors"
              :key="color"
              @click="selectColor(color)"
            >
              <div
                :class="{
                  'pano-wb-popup__item__colors__color-dot': true,
                  'pano-wb-popup__item__colors__color-dot--selected':
                    selectedColor === color
                }"
                :style="{
                  backgroundColor: color
                }"
              />
            </div>
          </div>
        </div>

        <div
          class="pano-wb-tb__item pano-withtip"
          slot="reference"
          data-tip="工具箱"
        >
          <span class="iconfont icon-tools" />
          <div
            class="pano-wb-tb__item__color-dot"
            :style="{ backgroundColor: selectedColor }"
          />
        </div>
      </Popover>

      <!-- 上传图片 -->
      <Popover placement="right" trigger="click">
        <div class="wb-img-inputs-wrapper">
          <div class="wb-img-wrapper">
            <div
              class="wb-img-wrapper__input-wrapper"
              :style="{ flexWrap: 'wrap' }"
            >
              <div class="wb-img-wrapper__header">图片元素</div>
              <Button
                shape="round"
                size="small"
                :style="{ marginTop: '5px' }"
                @click="uploadImage"
              >
                本地上传
              </Button>
            </div>

            <div class="wb-img-wrapper__divider" />

            <div class="wb-img-wrapper__input-wrapper">
              <div class="wb-img-wrapper__header">背景图片</div>
            </div>

            <div class="wb-img-wrapper__input-wrapper">
              <span class="wb-img-wrapper__label">背景图缩放模式</span>
              <label>
                <input
                  type="radio"
                  name="fillMode"
                  :value="Constants.WBImageScalingMode.Fit"
                  :checked="imgScaleMode === Constants.WBImageScalingMode.Fit"
                  @change="
                    () => {
                      this.imgScaleMode = Constants.WBImageScalingMode.Fit;
                      this.whiteboard.setBackgroundImageScalingMode(
                        Constants.WBImageScalingMode.Fit
                      );
                    }
                  "
                />
                自适应居中
              </label>
              <label>
                <input
                  type="radio"
                  name="fillMode"
                  :value="Constants.WBImageScalingMode.FillWidth"
                  :checked="
                    imgScaleMode === Constants.WBImageScalingMode.FillWidth
                  "
                  @change="
                    setScalingMode(Constants.WBImageScalingMode.FillWidth)
                  "
                />
                宽度满屏
              </label>
              <label>
                <input
                  type="radio"
                  name="fillMode"
                  :value="Constants.WBImageScalingMode.FillHeight"
                  :checked="
                    imgScaleMode === Constants.WBImageScalingMode.FillHeight
                  "
                  @change="
                    setScalingMode(Constants.WBImageScalingMode.FillHeight)
                  "
                />
                高度满屏
              </label>
            </div>

            <div class="wb-img-wrapper__input-wrapper">
              <span class="wb-img-wrapper__label">图片Url</span>
              <input type="text" v-model="imgUrl" />
              <Button
                shape="round"
                size="small"
                :style="{ marginLeft: '5px' }"
                @click="setBackgroundImageUrl"
              >
                设为背景
              </Button>
            </div>

            <div
              class="wb-img-wrapper__input-wrapper"
              :style="{ marginTop: '5px' }"
            >
              <Button shape="round" size="small" @click="upLoadBgImage">
                本地上传
              </Button>
            </div>
          </div>

          <div class="wb-img-wrapper">
            <div class="wb-img-wrapper__input-wrapper">
              <span class="wb-img-wrapper__label">图片Url列表</span>
              <input
                class="wb-img-wrapper__urls-input"
                type="text"
                v-model="imgUrlList"
                placeholder="url以空格隔开"
              />
            </div>
            <div class="wb-img-wrapper__input-wrapper">
              <span class="wb-img-wrapper__label">添加到当前文档</span>
              <input type="checkbox" v-model="bAddToCurrentDoc" />
              <Button
                shape="round"
                size="small"
                :style="{ marginLeft: '10px' }"
                @click="addImagBgList"
              >
                添加到白板
              </Button>
            </div>
          </div>
        </div>

        <div
          class="pano-wb-tb__item pano-withtip"
          data-tip="上传图片"
          slot="reference"
        >
          <span class="iconfont icon-images" />
          <div class="pano-wb-tb__item__triangle" />
        </div>
      </Popover>

      <!-- 橡皮擦 -->
      <Popover placement="right" trigger="click">
        <div class="pano-wb-popup">
          <div class="pano-wb-popup__item">
            <div
              :class="{
                'pano-wb-popup__item__select': true,
                'pano-wb-popup__item__select--selected':
                  insertType === Constants.ShapeType.Eraser
              }"
              @click="
                () => {
                  this.whiteboard.setToolType(Constants.ShapeType.Eraser);
                  this.insertType = Constants.ShapeType.Eraser;
                }
              "
            >
              <span class="iconfont icon-eraser" />
            </div>
            <div
              @click="
                () => {
                  this.whiteboard.setToolType(Constants.ShapeType.Delete);
                  this.insertType = Constants.ShapeType.Delete;
                }
              "
              :class="{
                'pano-wb-popup__item__select': true,
                'pano-wb-popup__item__select--selected':
                  insertType === Constants.ShapeType.Delete
              }"
            >
              <span class="iconfont icon-delete" />
            </div>
          </div>
          <div class="pano-wb-popup__item" :style="{ display: 'block' }">
            <Button
              size="small"
              type="text"
              :disabled="!isAdmin"
              :style="{ display: 'block', marginBottom: '5px' }"
              @click="clearAll"
            >
              清除所有内容
            </Button>
            <Button
              type="text"
              size="small"
              :disabled="!isAdmin"
              :style="{ display: 'block', marginBottom: '5px' }"
              @click="clearCurrentPage"
            >
              清除当前页
            </Button>
            <Button
              type="text"
              size="small"
              style="{ display: 'block', marginBottom: '5px' }"
              @click="clearMyDraws"
            >
              清除我的内容
            </Button>
            <Button
              type="text"
              size="small"
              style="{ display: 'block', marginBottom: '5px' }"
              @click="clearMyBg"
            >
              清除我的背景图
            </Button>
          </div>
        </div>

        <div
          class="pano-wb-tb__item pano-withtip"
          data-tip="橡皮擦"
          slot="reference"
        >
          <span class="iconfont icon-eraser" />
          <div class="pano-wb-tb__item__triangle" />
        </div>
      </Popover>

      <!-- 激光笔 -->
      <div
        :class="{
          'pano-wb-tb__item': true,
          'pano-withtip': true,
          'pano-wb-tb__item--selected':
            insertType === Constants.ShapeType.LaserPointer
        }"
        data-tip="激光笔"
        @click="setToolType(Constants.ShapeType.LaserPointer)"
      >
        <span class="iconfont icon-laser-pointer" style="font-size: 20px" />
      </div>

      <!-- 撤销 -->
      <div @click="undo" class="pano-wb-tb__item pano-withtip" data-tip="撤销">
        <span class="iconfont icon-undo" />
      </div>

      <!-- 重做 -->
      <div @click="redo" class="pano-wb-tb__item pano-withtip" data-tip="重做">
        <span class="iconfont icon-redo" />
      </div>

      <!-- 截图 -->
      <Popover placement="right" trigger="click">
        <div class="pano-wb-popup">
          <el-button
            size="small"
            :loading="snapshotingAll"
            :style="{ display: 'block', marginBottom: '5px' }"
            @click="snapshotAll"
          >
            截取全部内容
          </el-button>
          <el-button
            size="small"
            :loading="snapshotingView"
            :style="{ display: 'block', marginLeft: 0 }"
            @click="snapshotView"
          >
            仅截取可见区域
          </el-button>
        </div>

        <div
          class="pano-wb-tb__item pano-withtip"
          data-tip="截图"
          slot="reference"
        >
          <span class="iconfont icon-instagram" />
        </div>
      </Popover>

      <!-- 提示 -->
      <Popover placement="right" trigger="click">
        <div class="toolbar-instruction">
          <div class="toolbar-instruction__item">
            <div class="toolbar-instruction__item-title">
              设置样式：
            </div>
            <div class="toolbar-instruction__item-conetnt">
              单选中图形时，修改只对当前选中的图形生效；
              <br />
              没有选中图形时，修改全局样式，对接下来新绘制图形生效;
            </div>
          </div>
          <div class="toolbar-instruction__item">
            <div class="toolbar-instruction__item-title">
              缩放画板：
            </div>
            <div class="toolbar-instruction__item-conetnt">
              使用鼠标滚轮滚动，或者在触摸板上双指向上下推动
            </div>
          </div>
          <div class="toolbar-instruction__item">
            <div class="toolbar-instruction__item-title">
              拖动画板：
            </div>
            <div class="toolbar-instruction__item-conetnt">
              使用鼠标右键按住拖拽，或在触摸板上双指按下后拖动
            </div>
          </div>
          <div class="toolbar-instruction__item">
            <div class="toolbar-instruction__item-title">
              文档转码：
            </div>
            <div class="toolbar-instruction__item-conetnt">
              点击左上角导入文档按钮，支持 .docx, .xlsx, .pptx, .doc, .xls,
              .ppt, .pdf 等格式文件，转码成功后会自动跳转到文档
            </div>
          </div>
        </div>

        <div
          class="pano-wb-tb__item pano-withtip"
          data-tip="提示"
          slot="reference"
        >
          <span class="iconfont icon-info" />
        </div>
      </Popover>
    </div>

    <!-- 文档列表 -->
    <div class="wb-docs">
      <div
        v-for="(doc, index) in docs"
        :key="doc.docId"
        :class="{
          'wb-docs__item': true,
          'wb-docs__item--active': activeDocId === doc.docId
        }"
        @click="switchDoc(doc.docId)"
      >
        {{ index === 0 ? '默认白板' : doc.name }}
        <el-popconfirm
          v-if="index !== 0"
          title="确认删除文档?"
          @confirm="deleteDoc(doc.docId)"
          okText="是"
          cancelText="否"
        >
          <span
            class="iconfont icon-delete"
            :style="{ marginLeft: '3px', fontSize: '12px' }"
            slot="reference"
            @click.stop=""
          />
        </el-popconfirm>
      </div>

      <Popover placement="bottom" trigger="click">
        <div class="wb-docs__item wb-docs__add-doc-btn" slot="reference">
          <span
            class="iconfont icon-upload"
            :style="{ fontSize: '12px', marginRight: '3px' }"
          />
          上传文档
        </div>
        <div class="upload-doc-popover">
          <div
            class="upload-doc-popover__item"
            @click="uploadFileAndTranscode('doc')"
          >
            <div class="upload-doc-popover__item-title">文档转码</div>
            <div class="upload-doc-popover__item-content">
              将文档转为图片，支持 .docx, .xlsx, .pptx, .doc, .xls, .ppt, .pdf
              等格式
            </div>
          </div>
          <div
            class="upload-doc-popover__item"
            @click="uploadFileAndTranscode('courseware')"
          >
            <div class="upload-doc-popover__item-title">动态课件</div>
            <div class="upload-doc-popover__item-content">
              将 PPT 课件转为在线资源，支持动态交互，支持.pptx、.ppt格式
            </div>
          </div>
        </div>
      </Popover>
    </div>

    <!-- 右侧admin权限相关 -->
    <div class="wb-admin">
      <el-popover
        placement="bottom"
        width="150"
        v-model="visionFollowPopVisible"
        trigger="manual"
      >
        <div>
          <div>
            {{ followVisionUser.name || followVisionUser.userId }}
            正在分享视角，是否跟随？
          </div>
          <div>
            <el-button type="primary" size="mini" @click="confirmFllowVision">
              确认
            </el-button>
            <el-button size="mini" @click="rejectFllowVision">
              取消
            </el-button>
          </div>
        </div>
        <div class="wb-admin__admin-user" slot="reference">
          <el-tooltip
            :content="
              followVision
                ? `正在跟随${followVisionUser.name ||
                    followVisionUser.userId}的视角，点击关闭跟随`
                : `${followVisionUser.name || followVisionUser.userId}
                  正在分享视角，点击开始跟随`
            "
            placement="bottom"
          >
            <el-button
              v-show="followVisionUser.userId || followVision"
              type="default"
              size="mini"
              :style="{
                marginRight: '10px',
                ...(followVision
                  ? {
                      backgroundColor: '#73d13d',
                      borderColor: '#73d13d',
                      color: '#fff'
                    }
                  : {})
              }"
              @click="toggleVisionFllowing"
            >
              <i class="iconfont icon-eye" style="font-size: 12px" />
            </el-button>
          </el-tooltip>
        </div>
      </el-popover>

      <el-button
        :type="isSharingVision ? 'primary' : 'default'"
        size="mini"
        @click="toggleVisionSharing"
      >
        {{ isSharingVision ? '关闭视角跟随' : '开启视角跟随' }}
      </el-button>

      <el-tooltip
        content="隐藏轨迹后白板上绘制的轨迹将不再显示，但仍可和课件交互"
        placement="bottom"
      >
        <el-button
          :type="isWhiteboardHidden ? 'primary' : 'default'"
          size="mini"
          @click="toggleHidden"
        >
          {{ isWhiteboardHidden ? '隐藏轨迹' : '显示轨迹' }}
        </el-button>
      </el-tooltip>

      <span style="margin-left: 8px;">
        <span class="wb-admin__admin-user-label">{{isAdmin ? '我是admin':''}}</span>
      </span>
      <el-button
        size="mini"
        type="primary"
        @click="applyForWbAdmin"
        v-if="!isAdmin"
      >
        申请Admin权限
      </el-button>
    </div>

    <!-- 上传进度 -->
    <div class="wb-upload-progress" v-if="uploading">
      <div
        class="wb-upload-progress__loaded"
        :style="{ width: `${uploadProgress * 100}%` }"
      />
    </div>

    <!-- 分页按钮 -->
    <div class="pano-wb-pages pano-wb-tools">
      <div class="pano-wb-pages__wrapper">
        <button
          v-if="thumbUrls.length"
          class="pano-withtip pano-withtip-top pano-wb-pages__btn"
          data-tip="课件预览"
          :style="{ paddingRight: '6px' }"
          @click="setDrawerVisible(true)"
        >
          <span
            class="iconfont icon-files-empty"
            :style="{ fontSize: '14px' }"
          />
        </button>

        <button
          class="pano-withtip pano-withtip-top pano-wb-pages__btn"
          data-tip="前一页"
          :style="{ paddingLeft: '6px' }"
          @click="updatePageIndex(pageIndex - 1)"
        >
          <span
            class="iconfont icon-angle-double-left"
            :style="{ fontSize: '16px' }"
          />
        </button>
        <div class="pano-wb-pages__page-count">
          {{ `${pageIndex + 1} / ${pageCount}` }}
        </div>

        <button
          class="pano-withtip pano-withtip-top pano-wb-pages__btn"
          data-tip="后一页"
          @click="updatePageIndex(pageIndex + 1)"
        >
          <span
            class="iconfont icon-angle-double-right"
            :style="{ fontSize: '16px' }"
          />
        </button>

        <button
          v-if="activeDocId === 'default'"
          class="pano-withtip pano-withtip-top pano-wb-pages__btn"
          data-tip="填加一页"
          @click="addPage"
        >
          <span class="iconfont icon-plus" :style="{ fontSize: '10px' }" />
        </button>

        <button
          v-if="activeDocId === 'default'"
          class="pano-withtip pano-withtip-top pano-wb-pages__btn"
          data-tip="删除当前页"
          @click="removePage"
        >
          <span class="iconfont icon-minus" :style="{ fontSize: '10px' }" />
        </button>

        <Popover placement="top" trigger="click" style="min-width: 40px">
          <div class="pano-wb-pages__zoom-rate" slot="reference">
            {{ Math.round(scale * 100) }}%
          </div>
          <div :style="{ height: '120px' }">
            <el-slider
              :min="10"
              :max="500"
              vertical
              tooltipVisible
              :value="scale * 100"
              height="120px"
              @input="updateScaleFromSlider"
            />
          </div>
        </Popover>

        <button
          class="pano-withtip pano-withtip-top pano-wb-pages__btn"
          data-tip="重置缩放和画板位置"
          @click="resetScale"
        >
          <span class="iconfont icon-reset" :style="{ fontSize: '10px' }" />
        </button>
      </div>
    </div>

    <!-- 课件预览 -->
    <div
      class="wb-courseware-alide-mask"
      v-if="drawerVisible"
      @click="setDrawerVisible(false)"
    />
    <aside
      v-if="thumbUrls.length"
      :class="{
        'wb-courseware-alide': true,
        'wb-courseware-alide__hiden': !drawerVisible
      }"
    >
      <header>
        <span>课件预览</span>
        <i
          class="iconfont icon-close"
          :style="{ fontSize: '14px' }"
          @click="setDrawerVisible(false)"
        />
      </header>
      <div class="wb-courseware-list">
        <div
          v-for="(url, index) in thumbUrls"
          class="courseware-preview"
          :key="url"
        >
          <img
            :class="{
              'courseware-preview__active': pageIndex === index
            }"
            :src="url"
            alt="thumbnail"
            @click="updatePageIndex(index)"
          />
          <div class="courseware-index">
            <span>{{ index + 1 }}</span>
          </div>
        </div>
      </div>
    </aside>
  </div>
</template>

<script>
import { RtcWhiteboard, Constants } from '@pano.video/whiteboard';
import { get } from 'lodash-es';
import { Popover } from 'element-ui';

const colors = [
  'RGBA(220, 85, 76, 1.00)',
  'RGBA(238, 155, 78, 1.00)',
  'RGBA(250, 230, 115, 1.00)',
  'RGBA(153, 190, 103, 1.00)',
  'RGBA(84, 170, 233, 1.00)',
  'RGBA(69, 89, 170, 1.00)',
  'RGBA(141, 65, 165, 1.00)',
  'RGBA(239, 161, 157, 1.00)',
  'RGBA(255, 255, 255, 1.00)',
  'RGBA(155, 155, 153, 1.00)',
  'RGBA(81, 84, 91, 1.00)',
  'RGBA(0, 0, 0, 1.00)'
];

export default {
  data() {
    return {
      Constants,
      colors,
      imgUrl: 'https://developer.pano.video/download/example.jpeg',
      imgScaleMode: Constants.WBImageScalingMode.Fit,
      fontSize: this.whiteboard.fontSize,
      isAdmin: this.whiteboard.isAdmin,
      bold: this.whiteboard.bold,
      italic: this.whiteboard.italic,
      scale: this.whiteboard.scale,
      pageCount: this.whiteboard.getTotalNumberOfPages(),
      pageIndex: this.whiteboard.getCurrentPageNumber(),
      lineWidth: this.whiteboard.lineWidth,
      selectedColor: this.whiteboard.strokeStyle,
      insertType: this.whiteboard.getToolType(),
      fillType: this.whiteboard.fillType,
      docs: this.whiteboard.enumerateDocs(),
      activeDocId: this.whiteboard.activeDocId,
      activeDoc: this.whiteboard.activeDoc,
      imgUrlList: '',
      bAddToCurrentDoc: false,
      uploadProgress: 0,
      uploading: false,
      snapshotingView: false,
      snapshotingAll: false,
      drawerVisible: false,
      isSharingVision: false, // 是否正在共享视角
      followVision: false, //是否正在跟随别人的视角
      followVisionUser: { userId: '', name: '' }, // 正在跟随谁的视角
      visionFollowPopVisible: false, // 视角共享邀请提示框是否可见
      isWhiteboardHidden: false
    };
  },
  props: {
    whiteboard: {
      type: Object,
      required: true
    }
  },
  components: {
    Popover
  },
  computed: {
    thumbUrls() {
      // 课件预览图
      return this.activeDoc.courseware.thumbUrls || [];
    }
  },
  methods: {
    toggleHidden() {
      if (!this.isWhiteboardHidden) {
        this.whiteboard.hideWhiteboard();
      } else {
        this.whiteboard.showWhiteboard();
      }
      this.isWhiteboardHidden = !this.isWhiteboardHidden;
    },
    toggleVisionFllowing() {
      if (this.followVision) {
        this.whiteboard.stopFollowVision();
        this.followVision = false;
        this.visionFollowPopVisible = false;
      } else {
        this.whiteboard.startFollowVision();
        this.followVision = true;
      }
    },
    confirmFllowVision() {
      this.whiteboard.startFollowVision();
      this.followVision = true;
      this.visionFollowPopVisible = false;
    },
    rejectFllowVision() {
      this.visionFollowPopVisible = false;
    },
    toggleVisionSharing() {
      if (this.isSharingVision) {
        this.whiteboard.stopVisionShare();
        this.$message.success('您已停止视角分享');
      } else {
        this.$message.success('其他用户将收到您的视角跟随邀请');
        this.followVision = false;
        this.followVisionUser = { userId: '', name: '' };
        this.whiteboard.startVisionShare();
      }
      this.isSharingVision = !this.isSharingVision;
    },
    setDrawerVisible(drawerVisible) {
      this.drawerVisible = drawerVisible;
    },
    onRoleChanged() {
      this.isAdmin = this.whiteboard.isAdmin
    },
    /**
     * 从白板中获取最新的状态
     */
    getDataFromWhiteboard() {
      this.selectedColor = get(
        this.whiteboard.selectedShape,
        'shapeStyle.strokeStyle',
        this.whiteboard.strokeStyle
      );
      this.lineWidth = get(
        this.whiteboard.selectedShape,
        'shapeStyle.lineWidth',
        this.whiteboard.lineWidth
      );
      this.bold = get(
        this.whiteboard.selectedShape,
        'fontStyle.bold',
        this.whiteboard.bold
      );
      this.italic = get(
        this.whiteboard.selectedShape,
        'fontStyle.italic',
        this.whiteboard.italic
      );
      this.fontSize = get(
        this.whiteboard.selectedShape,
        'fontStyle.fontSize',
        this.whiteboard.fontSize
      );
      this.fillType = get(
        this.whiteboard.selectedShape,
        'shapeStyle.fillType',
        this.whiteboard.fillType
      );
      this.scale = this.whiteboard.scale;
      this.pageCount = this.whiteboard.getTotalNumberOfPages();
      this.pageIndex = this.whiteboard.getCurrentPageNumber();
      this.insertType = this.whiteboard.getToolType();
    },
    /**
     * 文档更新回调
     */
    onDocChanged() {
      if (this.activeDocId !== this.whiteboard.activeDocId) {
        this.activeDocId = this.whiteboard.activeDocId;
        this.$message.info(
          `已切换到${this.whiteboard.activeDoc.name ||
            this.whiteboard.activeDoc.docId}`
        );
      }
      this.docs = this.whiteboard.enumerateDocs().map((i) => ({
        ...i,
        name: get((i.name || '').split('.'), 0) || i.docId.toString().slice(-6)
      }));
      this.activeDoc = this.whiteboard.activeDoc;
    },
    checkIsAdmin() {
      if (!this.isAdmin) {
        this.$message.info('该操作需要admin权限，请点击右上角申请成为admin');
        return false;
      }
      return true;
    },
    async snapshotAll() {
      this.snapshotingAll = true;
      await this.whiteboard.snapshot(true, 'all');
      this.snapshotingAll = false;
    },
    async snapshotView() {
      this.snapshotingView = true;
      await this.whiteboard.snapshot(true, 'view');
      this.snapshotingView = false;
    },
    toggleItalic() {
      if (this.whiteboard.selectedShape) {
        this.whiteboard.setSelectedShapeFontStyle({
          italic: !this.italic
        });
      } else {
        this.whiteboard.italic = !this.italic;
      }
      this.italic = !this.italic;
    },
    toggleBold() {
      if (this.whiteboard.selectedShape) {
        this.whiteboard.setSelectedShapeFontStyle({
          bold: !this.bold
        });
      } else {
        this.whiteboard.bold = !this.bold;
      }
      this.bold = !this.bold;
    },
    setToolType(type) {
      this.whiteboard.setToolType(type);
      this.insertType = type;
      this.fillType = 'none';
      this.whiteboard.fillType = 'none';
    },
    setSolidToolType(type) {
      this.whiteboard.setToolType(type);
      this.insertType = type;
      this.fillType = 'color';
      this.whiteboard.fillType = 'color';
    },
    switchDoc(docId) {
      if (this.checkIsAdmin()) this.whiteboard.switchToDoc(docId);
    },
    deleteDoc(docId) {
      if (this.checkIsAdmin()) this.whiteboard.deleteDoc(docId);
    },
    addImagBgList() {
      if (!this.checkIsAdmin()) return;
      this.whiteboard.addBackgroundImages(
        this.imgUrlList.split(' '),
        this.bAddToCurrentDoc
      );
    },
    setLineWidth(newValue) {
      if (this.whiteboard.selectedShape) {
        this.whiteboard.setSelectedShapeStyle({ lineWidth: newValue });
      } else {
        this.whiteboard.lineWidth = newValue;
      }
      this.lineWidth = newValue;
    },
    setFontSize(newValue) {
      if (this.whiteboard.selectedShape) {
        this.whiteboard.setSelectedShapeFontStyle({
          fontSize: newValue
        });
      } else {
        this.whiteboard.fontSize = newValue;
      }
      this.fontSize = newValue;
    },
    uploadFileAndTranscode(type) {
      if (!this.checkIsAdmin()) return;
      const onUploadStateChange = (state) => {
        switch (state.code) {
          case 1:
            this.$message.info('正在转码...');
            this.uploading = false;
            this.uploadProgress = 0;
            break;
          case 2:
            break;
          case 3:
            this.uploading = false;
            this.uploadProgress = 0;
            this.$message.success('转码成功');
            break;
          case 4:
            if (state.uploadProgress.loaded === state.uploadProgress.total) {
              this.$message.success('文件上传成功，等待转码');
            }
            this.uploading =
              state.uploadProgress.loaded !== state.uploadProgress.total;
            this.uploadProgress =
              state.uploadProgress.loaded / state.uploadProgress.total;
            break;
          case 0:
            break;
          case -1:
            this.uploading = false;
            this.uploadProgress = 0;
            this.$message.error('文件过大，最大支持40M');
            break;
          case -2:
            this.uploading = false;
            this.uploadProgress = 0;
            this.$message.error('文件上传失败，请重试');
            break;
          case -3:
            this.uploading = false;
            this.uploadProgress = 0;
            this.$message.error('文件转码失败，请重试');
            break;
          default:
            break;
        }
      };
      this.whiteboard.uploadDoc(
        onUploadStateChange,
        type === 'doc' ? 'doc' : 'courseware',
        type === 'courseware'
      );
    },
    updatePageIndex(pageIndex) {
      if (!this.checkIsAdmin()) return;
      if (pageIndex < 0 || pageIndex >= this.whiteboard.getTotalNumberOfPages())
        return;
      this.whiteboard.gotoPage(pageIndex);
    },
    addPage() {
      if (!this.checkIsAdmin()) return;
      this.whiteboard.addPage();
    },
    removePage() {
      if (!this.checkIsAdmin()) return;
      this.whiteboard.removePage();
    },
    resetScale() {
      this.updateScale(1);
      this.whiteboard.setTranslate(0, 0);
    },
    updateScaleFromSlider(value) {
      this.updateScale(value / 100);
    },
    updateScale(nextScale) {
      try {
        if (nextScale > 0.1 && nextScale < 5) {
          this.whiteboard.setScale(nextScale);
          this.scale = nextScale;
        }
      } catch (error) {
        // console.log(error);
      }
    },
    uploadImage() {
      this.whiteboard.uploadImage((state) => {
        if (state.code === 1) {
          console.log('上传成功!');
        } else if (state.code === 2) {
          console.log(
            `已上传${(state.uploadProgress.loaded /
              state.uploadProgress.total) *
              100}%`
          );
        } else {
          this.$message.error('上传失败');
        }
      }, false);
    },
    selectColor(color) {
      if (this.whiteboard.selectedShape) {
        this.whiteboard.setSelectedShapeStyle({
          strokeStyle: color,
          fillStyle: color
        });
      } else {
        this.whiteboard.strokeStyle = color;
        this.whiteboard.fillStyle = color;
      }
      this.selectedColor = color;
    },
    undo() {
      this.whiteboard.undo();
    },
    redo() {
      this.whiteboard.redo();
    },
    upLoadBgImage() {
      if (!this.checkIsAdmin()) return;
      this.whiteboard.uploadImage((state) => {
        if (state.code === 1) {
          console.log('上传成功!');
        } else if (state.code === 2) {
          console.log(
            `已上传${(state.uploadProgress.loaded /
              state.uploadProgress.total) *
              100}%`
          );
        } else {
          this.$message.error('上传失败');
        }
      });
    },
    setScalingMode(mode) {
      this.imgScaleMode = mode;
      this.whiteboard.setBackgroundImageScalingMode(mode);
    },
    setBackgroundImageUrl() {
      if (!this.checkIsAdmin()) return;
      if (!this.imgUrl) {
        return;
      }
      this.whiteboard.setBackgroundImage(this.imgUrl);
    },
    clearAll() {
      if (!this.checkIsAdmin()) return;
      this.whiteboard.clearContents(false, Constants.WBClearType.All);
    },
    clearCurrentPage() {
      if (!this.checkIsAdmin()) return;
      this.whiteboard.clearContents(true, Constants.WBClearType.All);
    },
    clearMyDraws() {
      this.whiteboard.clearUserContents(this.whiteboard.userId, false, Constants.WBClearType.DRAWS);
    },
    clearMyBg() {
      this.whiteboard.clearUserContents(this.whiteboard.userId, false, Constants.WBClearType.BACKGROUND_IMAGE);
    },
    applyForWbAdmin() {
      this.whiteboard.setRoleType(Constants.WBRoleType.Admin)
    }
  },
  mounted() {
    this.whiteboard.on(
      RtcWhiteboard.Events.whiteboardContentUpdate,
      this.getDataFromWhiteboard
    );
    this.whiteboard.on(
      RtcWhiteboard.Events.userRoleTypeChanged,
      this.onRoleChanged
    );
    this.whiteboard.on(RtcWhiteboard.Events.docCreated, this.onDocChanged);
    this.whiteboard.on(RtcWhiteboard.Events.docSwitched, this.onDocChanged);
    this.whiteboard.on(RtcWhiteboard.Events.docDeleted, this.onDocChanged);
    this.whiteboard.on(RtcWhiteboard.Events.docUpdated, this.onDocChanged);
    // 某个人开启视角共享
    this.whiteboard.on(
      RtcWhiteboard.Events.userVisionShareStart,
      (userId, name) => {
        console.log('userVisionFollowStart event received');
        if (this.isSharingVision) {
          // 因为只能有一个人开启视角共享，当有人共享时需要把自己的状态关闭
          this.isSharingVision = false;
          this.$message.info(
            `${name || userId}正在分享视角，您的视角分享已自动取消`
          );
        }
        if (this.followVision) {
          // 正在跟随别人的视角，继续跟随
          this.followVisionUser = { name, userId };
          this.$message.info(`${name || userId}正在分享视角`);
        } else {
          this.followVisionUser = { name, userId };
          this.visionFollowPopVisible = true;
        }
      }
    );
    // 视角跟随结束
    this.whiteboard.on(
      RtcWhiteboard.Events.userVisionShareStop,
      (userId, name) => {
        console.log('onUserVisionFollowStop', userId, name);
        if (userId === this.followVisionUser.userId) {
          if (!this.isSharingVision) {
            this.$message.info(
              `${this.followVisionUser.name ||
                this.followVisionUser.userId}已经停止视角分享`
            );
          }
          this.followVision = false;
          this.followVisionUser = { name: '', userId: '' };
          this.visionFollowPopVisible = false;
        }
      }
    );
    this.onDocChanged();
  },
  beforeDestroy() {
    this.whiteboard.batchOff([
      RtcWhiteboard.Events.whiteboardContentUpdate,
      RtcWhiteboard.Events.userRoleTypeChanged,
      RtcWhiteboard.Events.docCreated,
      RtcWhiteboard.Events.docSwitched,
      RtcWhiteboard.Events.docDeleted,
      RtcWhiteboard.Events.docUpdated,
      RtcWhiteboard.Events.userVisionShareStart,
      RtcWhiteboard.Events.userVisionShareStop
    ]);
  }
};
</script>

<style lang="scss">
$deep-grey: #e6e6e6;
$light-grey: #f1f1f1;
$white-bg: #fff;
$font-color: #2b3139;
$hover-bg: #e9f4f8;
$fixed-left: 40px;
// $hover-bg: #f3f4f4;

.pano-wb-tb-wrapper {
  pointer-events: auto;
}

.el-popover {
  min-width: 40px !important;
}

.pano-whiteboard-tools-wrapper {
  font-family: Arial, SF Pro SC, SF Pro Text, SF Pro Icons, Helvetica Neue,
    Helvetica, Microsoft YaHei, sans-serif;
  -webkit-font-smoothing: antialiased;
}

.pano-wb-pages {
  position: absolute;
  z-index: 2;
  padding: 5px;
  right: 20px;
  bottom: 20px;
  font-size: 14px;
  color: $font-color;
  background-color: $white-bg;
  border-radius: 5px;
  box-shadow: 0 0 8px rgba(0, 0, 0, 0.1);
  &__zoom-rate {
    user-select: none;
    width: 40px;
    text-align: center;
  }
  &__wrapper {
    width: 100%;
    display: flex;
    justify-content: center;
    align-items: center;
  }
  &__wrapper ~ &__wrapper {
    margin-top: 4px;
  }
  &__page-count {
    text-align: center;
    user-select: none;
  }
  &__btn,
  &__btn-add {
    border: none;
    background: transparent;
    outline: none;
    padding: 5px 8px;
    cursor: pointer;
    display: flex;
    align-items: center;
    &--disabled {
      cursor: not-allowed;
      svg {
        fill: #c0c4cc;
      }
    }
  }
}

.wb-img-inputs-wrapper {
  background-color: #fff;
  font-size: 12px;
  padding: 0 8px;
  .wb-img-wrapper {
    margin: 8px 0;
    line-height: 30px;
    &__header {
      font-size: 18px;
      font-weight: bold;
      width: 100%;
      margin-bottom: 5px;
    }
    &__divider {
      height: 1px;
      background-color: #ddd;
      margin: 10px auto;
    }
    &__input-wrapper {
      display: flex;
      align-items: center;
    }
    &__label {
      font-weight: bold;
      margin-right: 10px;
      position: relative;
      & ~ &::before {
        position: absolute;
        left: -5px;
        top: 1px;
        content: '';
        height: 16px;
        width: 1px;
        background-color: #999;
      }
    }
    input[type='text'] {
      width: 200px;
      border: 1px solid #d5d5d5;
      border-radius: 3px;
      outline: none;
      height: 22px;
      padding: 0 4px;
    }
    label {
      display: inline-flex;
      align-items: center;
      input {
        margin: 0 4px;
      }
    }
  }
}

.wb-docs {
  position: absolute;
  top: 17px;
  left: 0;
  padding-left: 40px;
  display: flex;
  flex-wrap: wrap;
  z-index: 10;
  font-size: 12px;
  align-items: center;
  user-select: none;
  max-width: calc(100% - 520px);
  color: #555;
  &__item {
    display: flex;
    align-items: center;
    padding: 5px 16px;
    border-radius: 4px;
    box-shadow: 2px 2px 8px rgba(0, 0, 0, 0.2);
    cursor: pointer;
    margin-right: 8px;
    margin-bottom: 10px;
    background-color: #fff;
    min-height: 26px;
    flex-shrink: 0;
    position: relative;
    .icon-bin {
      position: absolute;
      visibility: visible;
      position: relative;
      right: -4px;
    }
  }
  &__item--active {
    color: #1eb1d3;
  }
  &__add-doc-btn {
    display: flex;
    align-items: center;
    justify-content: center;
    cursor: pointer;
    background-color: #fff;
    padding: 4px 10px;
    color: #555;
    span {
      color: #555;
    }
  }
}

.toolbar-instruction {
  list-style: none;
  max-width: 350px;
  padding: 10px 10px;
  color: #555;
  font-size: 12px;
  &__item {
    display: flex;
    &-title {
      min-width: 60px;
    }
  }
  &__item ~ &__item {
    margin-top: 10px;
  }
}

.pano-wb-tb * {
  user-select: none;
}

.pano-wb-tb {
  border-radius: 5px;
  font-size: 12px;
  padding: 8px;
  position: absolute;
  z-index: 2;
  left: 30px;
  top: 50%;
  transform: translateY(-50%);
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: center;
  user-select: none;
  font-size: 12px;
  box-shadow: 2px 2px 8px rgba(20, 14, 14, 0.2);
  color: #444;
  background-color: #fff;
  &__item {
    font-size: 18px;
    display: inline-flex;
    width: 30px;
    height: 30px;
    padding: 2px;
    justify-content: center;
    align-items: center;
    border-radius: 5px;
    cursor: pointer;
    margin: 3px 0;
    color: #666;
    position: relative;
    &:hover {
      background-color: $light-grey;
    }
    &--selected {
      background-color: $deep-grey;
      &:hover {
        background-color: $deep-grey;
      }
    }
    &__color-dot {
      position: absolute;
      right: 0;
      bottom: 3px;
      width: 6px;
      height: 6px;
      border-radius: 6px;
    }
    &__triangle {
      position: absolute;
      right: 2px;
      bottom: 3px;
      border: 3px solid transparent;
      border-right-color: #666;
      border-bottom-color: #666;
    }
  }
}

.pano-withtip {
  position: relative;
  &::after {
    content: attr(data-tip);
    position: absolute;
    // top:-30px;
    // right: -40px;
    // width: 100px;
    min-width: 80px;
    background-color: black;
    color: #fff;
    text-align: center;
    border-radius: 6px;
    padding: 5px;
    z-index: 100;
    transform: translateX(calc(90% - 10px));
    font-size: 12px;
    opacity: 0;
    transition: all 0.3s ease-in-out;
    display: none;
  }
  &:hover::after {
    display: block;
    opacity: 1;
    transform: translateX(90%);
  }
}
.pano-withtip-top {
  &::after {
    transform: translate(-50%, -30px);
    left: 50%;
  }
  &:hover::after {
    transform: translate(-50%, -40px);
  }
}

.pano-wb-popup {
  font-size: 12px;
  &__item {
    display: flex;
    align-items: center;
    flex-wrap: wrap;
    button {
      font-size: 12px !important;
    }
    &__label {
      font-size: 12px;
      padding-right: 5px;
    }
    &__select {
      font-size: 18px;
      display: inline-flex;
      width: 30px;
      height: 30px;
      padding: 2px;
      justify-content: center;
      align-items: center;
      border-radius: 3px;
      cursor: pointer;
      color: #666;
      position: relative;
      &:hover {
        background-color: $light-grey;
      }
      &--selected {
        background-color: $deep-grey;
        &:hover {
          background-color: $deep-grey;
        }
      }
    }
    &__select ~ &__select {
      margin-left: 5px;
    }
    &__colors {
      width: 25%;
      display: flex;
      justify-content: center;
      padding: 8px 0;
      &__color-dot {
        cursor: pointer;
        box-shadow: 0 0 6px rgba(0, 0, 0, 0.4);
        width: 20px;
        height: 20px;
        border-radius: 50%;
        transition: all 0.3s ease-in-out;
        position: relative;
        &:hover {
          box-shadow: 0 0 10px rgba(0, 0, 0, 0.6);
        }
        &--selected::before {
          position: absolute;
          left: -1px;
          top: -1px;
          width: 22px;
          height: 22px;
          border-radius: 50%;
          background-color: transparent;
          background-clip: content-box;
          border: 1px solid #fff;
          box-shadow: 0 0 8px rgba(0, 0, 0, 0.5);
          content: '';
        }
      }
    }
  }

  &__item ~ &__item {
    margin-top: 5px;
    position: relative;
    line-height: 32px;
    padding-top: 5px;
    &::before {
      position: absolute;
      content: '';
      width: 100%;
      height: 1px;
      background-color: #eee;
      top: 0;
    }
  }
}

.ant-popover-inner-content {
  padding: 12px;
}
// .ant-popover-arrow {
//   display: none;
// }
.ant-popover-inner {
  border-radius: 5px;
}

.wb-admin {
  position: absolute;
  right: 0;
  top: 16px;
  z-index: 10;
  font-size: 12px;
  padding: 5px 16px;
  min-height: 30px;
  display: flex;
  align-items: center;
  color: #333;
  user-select: none;
  height: 24px;
  button {
    margin-left: 5px;
    font-size: 12px !important;
  }
  &__admin-user-name {
    font-weight: bold;
  }
  &__admin-user-label {
    margin-left: 3px;
    color: #1890ff;
  }
}

.icon {
  width: 1em;
  height: 1em;
  vertical-align: -0.15em;
  fill: currentColor;
  overflow: hidden;
}

.loadingRotation {
  animation: rotation 2s infinite linear;
}
@keyframes rotation {
  0% {
    transform: rotate(0deg);
  }
  50% {
    transform: rotate(180deg);
  }
  100% {
    transform: rotate(360deg);
  }
}

.ant-popover-message-title {
  padding-left: 0 !important;
}

.ant-popover-message {
  padding-bottom: 6px;
}

.upload-doc-popover {
  margin: 0;
  width: 240px;
  &__item {
    padding: 4px 8px;
    transition: all 0.3s ease-in-out;
    border-radius: 5px;
    cursor: pointer;
    &:hover {
      background-color: #f5f5f5;
    }
    &-title {
      font-size: 14px;
      font-weight: bold;
      color: #333;
      margin-bottom: 5px;
    }
    &-content {
      font-size: 14px;
      color: #777;
      margin-bottom: 5px;
    }
  }
}

.wb-upload-progress {
  position: absolute;
  height: 4px;
  z-index: 1;
  top: 0;
  left: 0;
  right: 0;
  &__loaded {
    position: absolute;
    z-index: 2;
    left: 0;
    height: 100%;
    background-color: #4091f7;
    transition: width 0.3s ease-in-out;
  }
}

.wb-courseware-alide-mask {
  position: absolute;
  top: 0;
  right: 0;
  left: 0;
  opacity: 0;
  bottom: 0;
  transition: all 0.3s ease-in-out;
  z-index: 99;
}

.wb-courseware-alide {
  position: absolute;
  top: 0;
  right: 0;
  width: 260px;
  background-color: #f9f9f9;
  bottom: 0;
  box-shadow: rgba(0, 0, 0, 0.15) 0px 8px 24px 0px;
  transition: all 0.3s ease-in-out;
  z-index: 100;
  user-select: none;
  &__hiden {
    right: -260px;
  }
  header {
    height: 60px;
    display: flex;
    justify-content: space-between;
    font-size: 15px;
    padding: 20px 10px 0;
    span {
      font-weight: 500;
    }
    i {
      cursor: pointer;
    }
  }
  .wb-courseware-list {
    overflow-y: auto;
    height: calc(100% - 60px);
    .courseware-preview {
      margin: 20px 0;
      img {
        width: 230px;
        height: auto;
        border: 1px solid #fff;
        border-radius: 5px;
        margin: 0 auto;
        display: block;
        cursor: pointer;
      }
      img.courseware-preview__active {
        border-color: #4091f7;
      }
      .courseware-index {
        display: flex;
        justify-content: space-between;
        padding: 0 15px;
        margin-top: 5px;
        color: #777;
      }

      i {
        cursor: pointer;
      }
    }
  }
}
</style>
