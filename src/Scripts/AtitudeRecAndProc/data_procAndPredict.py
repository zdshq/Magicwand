import numpy as np
import torch
import torch.nn as nn
import os
import sys
import time

Data_Path = "./a/test.txt"
Model_Path = "../../network/ModelWight.pth"
device = "cuda:0"
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

def load_data(data_path):
    single_sample = []
    samples = []
    value = []
    with open(data_path, "r") as file:
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
                    break
    return np.array(samples).reshape(1, -1, 8)

def standardize(samples):
    features = []
    for single_sample in samples:
        mean = single_sample.mean(axis=0)
        std = single_sample.std(axis=0)
        standardized_data = (single_sample - mean) / std
        features.append(standardized_data)
    return np.array(features)

model = RNN().to(device=device)
model.load_state_dict(torch.load(Model_Path,  weights_only=True))
last_time = 0
while(True):
      if os.path.exists(Data_Path):
         current_time = os.path.getmtime(Data_Path)
         if last_time < current_time :
             print("new data comming!")
             time.sleep(7)
             current_time = os.path.getmtime(Data_Path)
             last_time = current_time
             data = load_data(Data_Path)
             data = standardize(data)
             data = torch.Tensor(data)
             data = data.to(device)
             y_hat = model(data)
             y_hat = torch.argmax(y_hat.mean(axis=1), 1)
             y_hat = np.array(y_hat)
             print(y_hat)
      time.sleep(1)