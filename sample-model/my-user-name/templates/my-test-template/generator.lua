--[[
  generator.lua - Used for generating multiple cards from a single note template.
  Usage: Return a function that returns a list of card data.

  A sufficiently complex scenario is Cloze-style notes.
  For clozes, each would need to be analyzed for cloze fragments, stored in an array for each field { [1]={{answer="...", hint="...", position=indexInString}, ...}, ... }
  For each element in the array, a card would be generated with {{cloze:FieldName}} subbing the matching cloze index for that card.
]]
local function findReplacement(str, afterIndex)
  afterIndex = afterIndex or 0;
end
local function ParseOutClozeFragments(fieldText)
  local subbedText = fieldText;
  local fragments = {};
  for cloze in fieldText:gmatch("{(%b{})}") do
    for index, answerText, hintIsProvided, hintText in fieldText:gmatch("{c(%d)::(.*)(::(.*))?}") do
      fragments[index] = fragments[index] or {};
      table.insert(fragments[index], {answer = answerText, hint = hintText});
    end
  end
  local uniqueIndex = 0;
  local uniqueFragments = {};
  for index, clozes in pairs(fragments) do
    for i = 0, #clozes do
      local cloze = clozes[i];
      local answer = cloze.answer;
      local hint = cloze.hint or "";
      if (hint) then
        hint = "::" .. hint;
      end
      local clozeText = string.format("{{c%d::%s%s}}", index, answer, hint);
      subbedText = subbedText:gsub(clozeText,
                                 function(match)
                                   local result = string.format("{{%d}}", uniqueIndex);
                                   uniqueIndex = uniqueIndex + 1;
                                   table.insert(uniqueFragments, match);
                                   return result;
                                 end
                                );
    end
  end
end
-- The only required function
local function generate(fields, frontTemplate, backTemplate)
  local cards = {}; -- List of cards to generate.
  for fieldID, fieldData in pairs(fields) do
    print(fieldData.Name, fieldData.Content);

  end
end
return generate;
