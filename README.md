# GarbageEye

Faster RCNN
YOLOv5
RetinaNet


# How RPN works

步骤总结：
1. 从 FPN 中获取特征图；
2. 将特征图输入 RPN head 得到特征图每一个网格对应锚框数量的置信度（前景还是背景的概率）和边界框回归参数；
3. 预测和训练
  - 预测
  - 把预测得到的边界框回归参数按照回归公式解析回来，得到左上角xy和右下角xy坐标；
  - filter_proposals 过滤预测框
    - 获取topk置信度的框->sigmoid->删除小框->移除较低置信度的框->NMS
  - 得到最终的预测框将会输入给 FastRCNN
  - 训练
    - 首先为targets选取一定数量的锚框样本（anchor模板），将这些样本分为正负样本，按照回归参数公式得到对应的回归参数
      - 其中正样本锚框与 GT box 的 IOU 大于0.7，负样本锚框的 IOU 小于0.3，0.3-0.7的锚框样本丢弃
      - 如果所有锚框与某个 GT box 的 IOU 都不超过0.7，则在所有的锚框中找出与其最大IOU的锚框作为该GT box的正样本
    - 平衡正负样本数量，在所有正负样本中一共选取256个，其中比例为0.5，如果正样本数量小于128，则用负样本补齐，反之亦然
  - 边界框回归损失：smoothl1损失
  - 类别损失：BCE 损失
