import numpy
import numpy as np
import torch
import torch.nn as nn
from torch.utils.data import Dataset, DataLoader,random_split

import re
import os
import sklearn
DataPath = "../../Dataset"
TestPath = "../../Dataset/TestData"
ModelWight = "model_weights.pth"
echos = 400

def load_data(folder_path):
    data = []
    labels = []
    single_sample = []
    samples = []
    value = []
    for filename in os.listdir(folder_path):
        if filename.endswith(".txt"):
            label = int(re.match(r'^(\w+)_\w+.txt$',filename).group(1)) - 2
            filepath = os.path.join(folder_path, filename)
            with open(filepath, "r") as file:
                for line in file:
                    line = line.strip()
                    if line.startswith("ACCELERO") or line.startswith("GYRO") \
                            or line.startswith("ACC ANGLE"):
                        if line.startswith("ACC ANGLE"):
                            value = [float(value) for value in line.split()[3::2]]
                        elif line.startswith("ACCELERO") or line.startswith("GYRO"):
                            value = [float(value) for value in line.split()[2::2]]
                    single_sample += value
                    value = []
                    if len(single_sample) == 8:
                        samples.append(single_sample)
                        single_sample = []
                        if len(samples) == 200:
                            data.append(samples)
                            labels.append(label)
                            samples = []
    return np.array(data),np.array(labels)

class AttitudeData(Dataset):

    def __init__(self, data, label):
        self.data = data
        self.label = label

    def __len__(self):
        return self.data.shape[0]

    def __getitem__(self, item):
        return self.data[item],self.label[item]

class RNN(nn.Module):

    def __init__(self, hidden_units=24, drop_rate=0.1, class_num = 2):
        super().__init__()
        self.class_num = class_num
        self.hidden_units = hidden_units
        self.drop = nn.Dropout(drop_rate)
        self.rnn = nn.GRU(8, hidden_units, 1, batch_first=True)
        self.linear = nn.Linear(hidden_units, class_num)

    def forward(self, data:torch.Tensor):

        batch,Tx,dim = data.shape[:]

        first_step = data.new_zeros(batch,1, dim)
        x = torch.cat((first_step, data), 1)
        x = x.float()
        hidden = torch.zeros(1 , batch, self.hidden_units, device=data.device)
        hidden = hidden.float()
        x = self.drop(x)
        output, hidden = self.rnn(x, hidden)
        y = self.linear(output)

        return y.reshape(batch,x.shape[1], self.class_num)

def standardize(samples):
    features = []
    for single_sample in samples:
        mean = single_sample.mean(axis=0)
        std = single_sample.std(axis=0)
        standardized_data = (single_sample - mean) / std
        features.append(standardized_data)
    return np.array(features)

device = 'cuda:0'

samples, labels = load_data(DataPath)
features = standardize(samples)
dataset = AttitudeData(features,labels)
train_size = int(len(dataset) * 0.9)
test_size = int(len(dataset) - train_size)
train_dataset, test_dataset = random_split(dataset, [train_size, test_size])
train_dataset = DataLoader(train_dataset, batch_size=2, shuffle=True)
test_dataset = DataLoader(test_dataset, batch_size=2, shuffle=True)
model = RNN().to(device='cuda:0')
optimizer = torch.optim.Adam(model.parameters(), lr=0.003)
criterion = torch.nn.CrossEntropyLoss()

for echo in range(echos):
    loss_sum = 0
    for x, y in train_dataset:
        x = x.to(device)
        y = y.to(device)
        y = y.to(torch.int64)
        hat_y = model(x)
        hat_y = hat_y.mean(axis=1)
        # 计算损失
        loss = criterion(hat_y, y)

        # 清零梯度
        optimizer.zero_grad()

        # 反向传播
        loss.backward()

        # 更新参数
        optimizer.step()

        # 记录损失
        loss_sum += loss.item()
    print(f'Epoch {echo + 1}/{echos}, Loss: {loss_sum / len(train_dataset)}')
accuracy = 0
with torch.no_grad():
    for (x, y) in test_dataset:
        x = x.to(device)
        y = y.to(device)
        y = y.to(torch.int64)
        y_hat = model(x)
        y_hat = torch.argmax(y_hat.mean(axis=1),1)
        accuracy += (y == y_hat).to(torch.float).sum()
accuracy_rate = accuracy/len(test_dataset)
print("accuracy_rate:" + str(torch.Tensor.cpu(accuracy_rate).numpy()))

torch.save(model, "model.pth")


