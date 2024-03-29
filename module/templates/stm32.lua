local class = require 'pl.class'
local Stm32Sequence = class()

function Stm32Sequence:_init(tStm32, tLog)
  self.tStm32 = tStm32
  self.tLog = tLog

  local vstruct = require "vstruct"
  self.tStructureReadData32 = vstruct.compile([[
    ucCommand:u1
    ulAddress:u4
  ]])

  self.tStructureWriteData32 = vstruct.compile([[
    ucCommand:u1
    ulAddress:u4
    ulData:u4
  ]])

  self.tStructureRmwData32 = vstruct.compile([[
    ucCommand:u1
    ulAddress:u4
    ulAnd:u4
    ulOr:u4
  ]])

  self.tStructurePollData32 = vstruct.compile([[
    ucCommand:u1
    ulAddress:u4
    ulAnd:u4
    ulCmp:u4
    ulTimeoutInMs:u4
  ]])

  self.tStructureHashMemory = vstruct.compile([[
    ucCommand:u1
    ulAddress:u4
    ulSize:u4
  ]])

  self.tSequence = { readsize = 0 }
end


function Stm32Sequence:read_data32(ulAddress)
  local strBin
  strBin = self.tStructureReadData32:write{
    ucCommand = self.tStm32.STM32_COMMAND_ReadData32,
    ulAddress = ulAddress
  }

  local tSequence = self.tSequence
  table.insert(tSequence, strBin)
  tSequence.readsize = tSequence.readsize + 4
end


function Stm32Sequence:write_data32(ulAddress, ulData)
  local strBin
  strBin = self.tStructureWriteData32:write{
    ucCommand = self.tStm32.STM32_COMMAND_WriteData32,
    ulAddress = ulAddress,
    ulData = ulData
  }

  local tSequence = self.tSequence
  table.insert(tSequence, strBin)
end


function Stm32Sequence:rmw_data32(ulAddress, ulAnd, ulOr)
  local strBin
  strBin = self.tStructureRmwData32:write{
    ucCommand = self.tStm32.STM32_COMMAND_RmwData32,
    ulAddress = ulAddress,
    ulAnd = ulAnd,
    ulOr = ulOr
  }

  local tSequence = self.tSequence
  table.insert(tSequence, strBin)
end


function Stm32Sequence:poll_data32(ulAddress, ulAnd, ulCmp, ulTimeoutInMs)
  local strBin
  strBin = self.tStructurePollData32:write{
    ucCommand = self.tStm32.STM32_COMMAND_PollData32,
    ulAddress = ulAddress,
    ulAnd = ulAnd,
    ulCmp = ulCmp,
    ulTimeoutInMs = ulTimeoutInMs
  }

  local tSequence = self.tSequence
  table.insert(tSequence, strBin)
end


function Stm32Sequence:hash_memory(ulAddress, ulSize)
  local strBin
  strBin = self.tStructureHashMemory:write{
    ucCommand = self.tStm32.STM32_COMMAND_HashMemory,
    ulAddress = ulAddress,
    ulSize = ulSize
  }

  local tSequence = self.tSequence
  table.insert(tSequence, strBin)
  tSequence.readsize = tSequence.readsize + 48
end


function Stm32Sequence:run()
  local tSequence = self.tSequence

  -- Get the sequence data.
  local strSequence = table.concat(tSequence)
  -- Get the size of the result data.
  local sizResultData = tSequence.readsize

  return self.tStm32:__sequence_run(strSequence, sizResultData)
end



local AppBridgeModuleStm32 = class()

function AppBridgeModuleStm32:_init(tAppBridge, tLog)
  self.pl = require'pl.import_into'()

  self.tAppBridge = tAppBridge
  self.tLog = tLog

  -- This is the path to the module binary.
  self.strModulePath = 'netx/netx90_app_bridge_module_stm32.bin'

  -- TODO: Get this from the binary.
  self.ulModuleLoadAddress = 0x000B8000
  self.ulModuleExecAddress = 0x000B8001
  self.ulModuleBufferArea = 0x000BC000

  self.STM32_COMMAND_Initialize = ${STM32_COMMAND_Initialize}
  self.STM32_COMMAND_ReadData32 = ${STM32_COMMAND_ReadData32}
  self.STM32_COMMAND_WriteData32 = ${STM32_COMMAND_WriteData32}
  self.STM32_COMMAND_WriteArea = ${STM32_COMMAND_WriteArea}
  self.STM32_COMMAND_RmwData32 = ${STM32_COMMAND_RmwData32}
  self.STM32_COMMAND_PollData32 = ${STM32_COMMAND_PollData32}
  self.STM32_COMMAND_HashMemory = ${STM32_COMMAND_HashMemory}
  self.STM32_COMMAND_RunSequence = ${STM32_COMMAND_RunSequence}
  self.STM32_COMMAND_ExtendedEraseFlashPage = ${STM32_COMMAND_ExtendedEraseFlashPage}
end


function AppBridgeModuleStm32:initialize(ulOnlyActivateBootloader)
  local tAppBridge = self.tAppBridge
  local tLog = self.tLog
  -- The default is to install the stub.
  ulOnlyActivateBootloader = ulOnlyActivateBootloader or 0

  -- Download the Stm32 module.
  local tModuleData, strError = self.pl.utils.readfile(self.strModulePath, true)
  if tModuleData==nil then
    tLog.error('Failed to load the module from "%s": %s', self.strModulePath, strError)
    error('Failed to load module.')
  end
  tAppBridge:write_area(self.ulModuleLoadAddress, tModuleData)

  -- Initialize the UART connection to the STM32.
  tLog.info('STM32 initialize')
  local ulValue = tAppBridge:call(self.ulModuleExecAddress, self.STM32_COMMAND_Initialize, ulOnlyActivateBootloader)
  if ulValue~=0 then
    tLog.error('Failed to initialize the STM32 module: %s', tostring(ulValue))
    error('Failed to initialize the STM32 module.')
  end
end


function AppBridgeModuleStm32:read_data32(ulAddress)
  local tAppBridge = self.tAppBridge
  local tLog = self.tLog

  local ulResult = tAppBridge:call(self.ulModuleExecAddress, self.STM32_COMMAND_ReadData32, ulAddress, self.ulModuleBufferArea)
  if ulResult~=0 then
    tLog.error('Failed to read STM32[0x%08x] : %d', ulAddress, ulResult)
    error('Failed to read.')
  end
  local ulValue = tAppBridge:read_register(self.ulModuleBufferArea)
  return ulValue
end


function AppBridgeModuleStm32:write_data32(ulAddress, ulData)
  local tAppBridge = self.tAppBridge
  local tLog = self.tLog

  local ulResult = tAppBridge:call(self.ulModuleExecAddress, self.STM32_COMMAND_WriteData32, ulAddress, ulData)
  if ulResult~=0 then
    tLog.error('Failed to write STM32[0x%08x]=0x%08x : %d', ulAddress, ulData, ulResult)
    error('Failed to write.')
  end
end


function AppBridgeModuleStm32:write_area(ulAddress, strData)
  local tLog = self.tLog
  local tAppBridge = self.tAppBridge
  local tResult

  local sizData = string.len(strData)
  if sizData==0 then
    tLog.debug('Ignoring empty write request.')
    tResult = true
  else
    -- Copy the data to the APP buffer.
    tResult = tAppBridge:write_area(self.ulModuleBufferArea, strData)
    if tResult~=true then
      tLog.error('Failed to write the data to the APP memory.')
      error('Failed to write the data to the APP memory.')
    else
      -- Run the sequence.
      local ulResult = tAppBridge:call(self.ulModuleExecAddress, self.STM32_COMMAND_WriteArea, ulAddress, self.ulModuleBufferArea, sizData)
      if ulResult~=0 then
        tLog.error('Failed to write the area : %d', ulResult)
        error('Failed to write the area.')
      else
        tResult = true
      end
    end
  end

  return tResult
end


function AppBridgeModuleStm32:rmw_data32(ulAddress, ulAnd, ulOr)
  local tAppBridge = self.tAppBridge
  local tLog = self.tLog

  local ulResult = tAppBridge:call(self.ulModuleExecAddress, self.STM32_COMMAND_RmwData32, ulAddress, ulAnd, ulOr)
  if ulResult~=0 then
    tLog.error('Failed to write STM32[0x%08x]=(STM32[0x%08x] AND 0x%08x) OR 0x%08x : %d', ulAddress, ulAddress, ulAnd, ulOr, ulResult)
    error('Failed to rmw.')
  end
end


function AppBridgeModuleStm32:poll_data32(ulAddress, ulAnd, ulCmp, ulTimeoutInMs)
  local tAppBridge = self.tAppBridge
  local tLog = self.tLog

  -- The "poll" command exceeds the available parameters of the "go" command.
  -- The data must be stored somewhere else.
  local ulParameter = self.ulModuleBufferArea
  tAppBridge:write_register(ulParameter,      ulAddress)
  tAppBridge:write_register(ulParameter+0x04, ulAnd)
  tAppBridge:write_register(ulParameter+0x08, ulCmp)
  tAppBridge:write_register(ulParameter+0x0c, ulTimeoutInMs)

  local ulResult = tAppBridge:call(self.ulModuleExecAddress, self.STM32_COMMAND_PollData32, ulParameter)
  if ulResult~=0 then
    tLog.error('Failed to poll (STM32[0x%08x] AND 0x%08x) == 0x%08x in %dms : %d', ulAddress, ulAnd, ulCmp, ulTimeoutInMs, ulResult)
    error('Failed to poll.')
  end
end


function AppBridgeModuleStm32:hash_memory(ulAddress, ulSize)
  local tAppBridge = self.tAppBridge
  local tLog = self.tLog

  local ulResult = tAppBridge:call(self.ulModuleExecAddress, self.STM32_COMMAND_HashMemory, ulAddress, ulSize, self.ulModuleBufferArea)
  if ulResult~=0 then
    tLog.error('Failed to hash STM32[0x%08x, 0x%08x[ : %d', ulAddress, ulAddress+ulSize, ulResult)
    error('Failed to read.')
  end
  local strHash = tAppBridge:read_area(self.ulModuleBufferArea, 48)
  return strHash
end


function AppBridgeModuleStm32:sequence_create()
  return Stm32Sequence(self, self.tLog)
end


function AppBridgeModuleStm32:__sequence_run(strSequence, sizResultData)
  local tLog = self.tLog
  local tAppBridge = self.tAppBridge
  local tResult

  local sizSequence = string.len(strSequence)
  if sizSequence==0 then
    tLog.debug('Ignoring empty sequence.')
    tResult = true
  else
    -- Copy the sequence to the APP buffer.
    tResult = tAppBridge:write_area(self.ulModuleBufferArea, strSequence)
    if tResult~=true then
      tLog.error('Failed to write the sequence data.')
      error('Failed to write the sequence data.')
    else
      -- Run the sequence.
      local ulResult = tAppBridge:call(self.ulModuleExecAddress, self.STM32_COMMAND_RunSequence, sizSequence)
      if ulResult~=0 then
        tLog.error('Failed to execute the sequence : %d', ulResult)
        error('Failed to execute the sequence.')
      else
        if sizResultData==0 then
          tResult = true
        else
          tResult = tAppBridge:read_area(self.ulModuleBufferArea, sizResultData)
        end
      end
    end
  end

  return tResult
end


function AppBridgeModuleStm32:extended_erase_flash_page(usFlashPage)
  local tAppBridge = self.tAppBridge
  local tLog = self.tLog

  local ulResult = tAppBridge:call(self.ulModuleExecAddress, self.STM32_COMMAND_ExtendedEraseFlashPage, usFlashPage)
  if ulResult~=0 then
    tLog.error('Failed to erase flash page %d: %d', usFlashPage, ulResult)
    error('Failed to erase.')
  end
end


return AppBridgeModuleStm32
