import runMqtt.operations.getRSSI as getRSSI
import runMqtt.operations.getMac as getMac
import runMqtt.operations.getIP as getIP
import runMqtt.operations.getVersion as getVersion
import runMqtt.operations.getRole as getRole
import runMqtt.operations.getError as getError
import runMqtt.operations.getInfo as getInfo
import runMqtt.operations.getOn as getOn
import runMqtt.operations.getShape as getShape
import runMqtt.operations.getControl as getControl
import runMqtt.operations.getUDP as getUDP


commands = {
   'DST:': getRSSI.getRSSI,
   'MAC:': getMac.getMac,
   'IP:': getIP.getIP,
   'VER:': getVersion.getVersion,
   'ROLE:': getRole.getRole,
   'ERR:': getError.getError,
   'INFO:': getInfo.getInfo,
   'ON:': getOn.getOn,
   'SHAPE:': getShape.getShape,
   'CONTROL:': getControl.getControl,
   'UDP:': getUDP.startUDP,
}   
