vim.opt.makeprg = "cmake --build build-debug"

-- Toggle between src/<year>/day<NN>.cc and inputs/<year>/input<NN>
local function toggle_aoc_input()
  local path = vim.fn.expand('%:p')
  local root = vim.fn.expand('%:p:h:h:h')
  local year, day = path:match('/src/(%d+)/day(%d+)%.cc$')
  if year and day then
    vim.cmd('edit ' .. vim.fn.fnameescape(root .. '/inputs/' .. year .. '/input' .. day))
    return
  end
  year, day = path:match('/inputs/(%d+)/input(%d+)$')
  if year and day then
    vim.cmd('edit ' .. vim.fn.fnameescape(root .. '/src/' .. year .. '/day' .. day .. '.cc'))
    return
  end
  vim.notify('Not an AoC source or input file', vim.log.levels.WARN)
end

vim.keymap.set('n', '<leader>ai', toggle_aoc_input, { desc = 'Toggle AoC input/source' })
