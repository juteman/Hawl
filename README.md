# Hawl 渲染引擎
目标是集成Dx12 和 Vulkan 这两种现代API的渲染引擎


## 计划实现 TODOLIST
### 基础设施
- [x] 高精度低精度Timer 封装。 TODO：后期准备采用linux内核时间轮类似算法实现tick定时器 (进行中)
- [x] 智能指针(已更新SharedPtr)
- [ ] 线程池实现 (进行中)
- [ ] Log 封装
- [ ] 内存跟踪
- [ ] 跨平台

### 窗体
- [x] Windows 窗体封装
- [ ] Linux 窗体封装

### Renderer
- [ ] DX12 封装 (进行中)
- [ ] Vulkan 封装

### 待定
- [ ] 加入反射
- [ ] 尝试加入GC，(并发标记摧毁)