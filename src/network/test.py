import torch
import torch.nn as nn
import torch.optim as optim

# 生成一些随机数据，假设我们有一个线性函数 y = 2x + 3
torch.manual_seed(42)
X = torch.randn(100, 1)  # 100 个样本，1 个特征
y = 2 * X + 3 + 0.1 * torch.randn(100, 1)  # 添加一些噪声


# 定义一个简单的线性模型
class LinearModel(nn.Module):
    def __init__(self):
        super(LinearModel, self).__init__()
        self.linear = nn.Linear(1, 1)

    def forward(self, x):
        print(x.shape)
        return self.linear(x)


# 实例化模型、定义损失函数和优化器
model = LinearModel()
criterion = nn.MSELoss()
optimizer = optim.SGD(model.parameters(), lr=0.01)

# 设置批次大小
batch_size = 16
n_batches = len(X) // batch_size

# 开始训练
n_epochs = 100
for epoch in range(n_epochs):
    for i in range(n_batches):
        # 取出一个 batch 的数据
        X_batch = X[i * batch_size:(i + 1) * batch_size]
        y_batch = y[i * batch_size:(i + 1) * batch_size]

        # 前向传播
        outputs = model(X_batch)
        loss = criterion(outputs, y_batch)

        # 反向传播和优化
        optimizer.zero_grad()
        loss.backward()
        optimizer.step()

    if (epoch + 1) % 10 == 0:
        print(f'Epoch [{epoch + 1}/{n_epochs}], Loss: {loss.item():.4f}')

# 打印模型参数
print(f'Learned parameters: {model.linear.weight.item():.4f}, {model.linear.bias.item():.4f}')
