clear all;
%Toolbox:
%https://ch.mathworks.com/matlabcentral/fileexchange/64303-mqtt-in-matlab
mqttHost=mqtt('tcp://192.168.1.2','Port',1883, 'ClientID','MAALAA');
mySub=subscribe(mqttHost,'+/p');

topic = 'Ext/g';
message = 'hello';

publish(mqttHost, topic, message);

topic = 'Ext/K/l';
message = 'led 10 10 10';
publish(mqttHost, topic, message);

pause(1)
messageTable=readall(mySub);