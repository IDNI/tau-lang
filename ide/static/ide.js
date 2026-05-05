/**
 * Tau-lang IDE — Main application logic
 */
const IDE = {
  // State
  tabs: {},
  activeTab: null,
  replHistory: [],
  replHistoryIdx: -1,
  helpData: null,
  currentHelpTopic: null,
  wsRepl: null,
  bottomVisible: true,
  sidebarVisible: true,
  commands: [],
  _autocompleteVisible: false,

  // -----------------------------------------------------------------------
  // Initialization
  // -----------------------------------------------------------------------
  async init() {
    // Load grammar-derived tokens before initializing editor
    await TauSyntax.loadFromGrammar();
    this.initEditor('untitled', '# Welcome to the Tau-lang IDE\n# Type a formula and press Ctrl+Enter to evaluate\n# Press Shift+Enter to run as specification\n# Press F1 for help\n\n');
    this.initKeyboard();
    this.initRepl();
    this.initResizer();
    this.initCommands();
    this.initTooltip();
    await this.loadExamples();
    await this.loadHelp();
    this.loadTemplates();
    this.checkStatus();
    this.toast('Tau-lang IDE ready', 'info');
  },

  // -----------------------------------------------------------------------
  // Editor
  // -----------------------------------------------------------------------
  initEditor(tabId, content) {
    const editorEl = document.getElementById('editor');
    editorEl.innerHTML = '';

    // Create container
    const wrapper = document.createElement('div');
    wrapper.style.position = 'relative';
    wrapper.style.width = '100%';
    wrapper.style.height = '100%';

    // Line numbers
    const lineNums = document.createElement('div');
    lineNums.className = 'line-numbers';
    lineNums.id = 'line-numbers';

    // Highlight overlay
    const overlay = document.createElement('div');
    overlay.className = 'highlight-overlay';
    overlay.id = 'highlight-overlay';

    // Textarea
    const textarea = document.createElement('textarea');
    textarea.className = 'editor-textarea';
    textarea.id = 'editor-textarea';
    textarea.value = content || '';
    textarea.spellcheck = false;
    textarea.autocomplete = 'off';
    textarea.style.paddingLeft = '56px';

    wrapper.appendChild(lineNums);
    wrapper.appendChild(overlay);
    wrapper.appendChild(textarea);
    editorEl.appendChild(wrapper);

    // Events
    textarea.addEventListener('input', () => this.onEditorChange());
    textarea.addEventListener('scroll', () => this.syncScroll());
    textarea.addEventListener('keydown', (e) => this.onEditorKeydown(e));
    textarea.addEventListener('click', () => this.updateCursorPos());
    textarea.addEventListener('keyup', () => this.updateCursorPos());

    // Store tab
    this.tabs[tabId] = { id: tabId, name: tabId + '.tau', content: content || '', modified: false };
    this.activeTab = tabId;

    this.onEditorChange();
  },

  getEditor() {
    return document.getElementById('editor-textarea');
  },

  onEditorChange() {
    const textarea = this.getEditor();
    if (!textarea) return;
    const source = textarea.value;

    // Update tab content
    if (this.activeTab && this.tabs[this.activeTab]) {
      this.tabs[this.activeTab].content = source;
    }

    // Update highlighting
    const overlay = document.getElementById('highlight-overlay');
    if (overlay) {
      overlay.innerHTML = TauSyntax.highlight(source) + '\n';
    }

    // Update line numbers
    this.updateLineNumbers(source);
    this.syncScroll();
    this.updateCursorPos();
  },

  updateLineNumbers(source) {
    const lineNums = document.getElementById('line-numbers');
    if (!lineNums) return;
    const count = (source.match(/\n/g) || []).length + 1;
    let html = '';
    for (let i = 1; i <= count; i++) {
      html += i + '\n';
    }
    lineNums.textContent = html;
  },

  syncScroll() {
    const textarea = this.getEditor();
    const overlay = document.getElementById('highlight-overlay');
    const lineNums = document.getElementById('line-numbers');
    if (textarea && overlay) {
      overlay.scrollTop = textarea.scrollTop;
      overlay.scrollLeft = textarea.scrollLeft;
    }
    if (textarea && lineNums) {
      lineNums.scrollTop = textarea.scrollTop;
    }
  },

  updateCursorPos() {
    const textarea = this.getEditor();
    if (!textarea) return;
    const pos = textarea.selectionStart;
    const text = textarea.value.substring(0, pos);
    const lines = text.split('\n');
    const ln = lines.length;
    const col = lines[lines.length - 1].length + 1;
    document.getElementById('cursor-pos').textContent = `Ln ${ln}, Col ${col}`;
  },

  onEditorKeydown(e) {
    const textarea = this.getEditor();

    // Tab key — insert spaces
    if (e.key === 'Tab' && !e.shiftKey) {
      e.preventDefault();
      const start = textarea.selectionStart;
      const end = textarea.selectionEnd;
      textarea.value = textarea.value.substring(0, start) + '    ' + textarea.value.substring(end);
      textarea.selectionStart = textarea.selectionEnd = start + 4;
      this.onEditorChange();
      return;
    }

    // Ctrl+Enter — eval current line or selection
    if (e.key === 'Enter' && e.ctrlKey) {
      e.preventDefault();
      this.evalSelection();
      return;
    }

    // Shift+Enter — run entire program
    if (e.key === 'Enter' && e.shiftKey) {
      e.preventDefault();
      this.runProgram();
      return;
    }

    // Ctrl+/ — toggle comment
    if (e.key === '/' && e.ctrlKey) {
      e.preventDefault();
      this.toggleComment();
      return;
    }

    // Ctrl+Space — autocomplete
    if (e.key === ' ' && e.ctrlKey) {
      e.preventDefault();
      this.showAutocomplete();
      return;
    }
  },

  toggleComment() {
    const textarea = this.getEditor();
    const start = textarea.selectionStart;
    const end = textarea.selectionEnd;
    const text = textarea.value;

    // Find line boundaries
    let lineStart = text.lastIndexOf('\n', start - 1) + 1;
    let lineEnd = text.indexOf('\n', end);
    if (lineEnd === -1) lineEnd = text.length;

    const line = text.substring(lineStart, lineEnd);
    const isCommented = line.trimStart().startsWith('#');

    let newLine;
    if (isCommented) {
      newLine = line.replace(/^(\s*)# ?/, '$1');
    } else {
      newLine = line.replace(/^(\s*)/, '$1# ');
    }

    textarea.value = text.substring(0, lineStart) + newLine + text.substring(lineEnd);
    textarea.selectionStart = start + (newLine.length - line.length);
    textarea.selectionEnd = textarea.selectionStart;
    this.onEditorChange();
  },

  showAutocomplete() {
    const textarea = this.getEditor();
    const completions = TauSyntax.getCompletions(textarea.value, textarea.selectionStart);
    if (completions.length === 0) return;

    // Simple autocomplete: insert first match or show list
    // For now, show toast with options
    const items = completions.map(c => `${c.label} (${c.type})`).join(', ');
    this.toast(`Completions: ${items}`, 'info');
  },

  // -----------------------------------------------------------------------
  // File Operations
  // -----------------------------------------------------------------------
  newFile() {
    const id = 'untitled-' + Date.now();
    const name = 'untitled.tau';

    // Add tab
    this.tabs[id] = { id, name, content: '', modified: false };
    this.addTabUI(id, name);
    this.switchTab(id);
  },

  openFile() {
    const input = document.createElement('input');
    input.type = 'file';
    input.accept = '.tau,.txt';
    input.onchange = async (e) => {
      const file = e.target.files[0];
      if (!file) return;
      const text = await file.text();
      const id = 'file-' + Date.now();
      this.tabs[id] = { id, name: file.name, content: text, modified: false };
      this.addTabUI(id, file.name);
      this.switchTab(id);
    };
    input.click();
  },

  saveFile() {
    const tab = this.tabs[this.activeTab];
    if (!tab) return;
    const blob = new Blob([tab.content], { type: 'text/plain' });
    const a = document.createElement('a');
    a.href = URL.createObjectURL(blob);
    a.download = tab.name;
    a.click();
    URL.revokeObjectURL(a.href);
    tab.modified = false;
    this.toast(`Saved ${tab.name}`, 'success');
  },

  // -----------------------------------------------------------------------
  // Tabs
  // -----------------------------------------------------------------------
  addTabUI(id, name) {
    const tabBar = document.getElementById('tab-bar');
    const tab = document.createElement('div');
    tab.className = 'tab';
    tab.dataset.id = id;
    tab.onclick = () => this.switchTab(id);
    tab.innerHTML = `
      <span class="tab-title">${escHtml(name)}</span>
      <span class="tab-close" onclick="event.stopPropagation(); IDE.closeTab('${id}')">&times;</span>
    `;
    tabBar.appendChild(tab);
  },

  switchTab(id) {
    if (!this.tabs[id]) return;
    this.activeTab = id;
    const textarea = this.getEditor();
    if (textarea) {
      textarea.value = this.tabs[id].content;
      this.onEditorChange();
    }

    // Update tab UI
    document.querySelectorAll('#tab-bar .tab').forEach(t => {
      t.classList.toggle('active', t.dataset.id === id);
    });
  },

  closeTab(id) {
    if (Object.keys(this.tabs).length <= 1) return; // Keep at least one tab
    delete this.tabs[id];
    const tabEl = document.querySelector(`#tab-bar .tab[data-id="${id}"]`);
    if (tabEl) tabEl.remove();
    if (this.activeTab === id) {
      const remaining = Object.keys(this.tabs);
      if (remaining.length > 0) this.switchTab(remaining[0]);
    }
  },

  // -----------------------------------------------------------------------
  // Run & Eval
  // -----------------------------------------------------------------------
  async evalSelection() {
    const textarea = this.getEditor();
    let cmd = '';

    // Use selection if available, otherwise current line
    if (textarea.selectionStart !== textarea.selectionEnd) {
      cmd = textarea.value.substring(textarea.selectionStart, textarea.selectionEnd);
    } else {
      const text = textarea.value;
      const pos = textarea.selectionStart;
      let lineStart = text.lastIndexOf('\n', pos - 1) + 1;
      let lineEnd = text.indexOf('\n', pos);
      if (lineEnd === -1) lineEnd = text.length;
      cmd = text.substring(lineStart, lineEnd);
    }

    cmd = cmd.trim();
    if (!cmd) return;

    // Add period if missing (REPL commands need it)
    if (!cmd.endsWith('.')) cmd += '.';

    this.setStatus('busy', 'Evaluating...');
    this.appendRepl(cmd, 'cmd');

    try {
      const resp = await fetch('/api/eval', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ command: cmd }),
      });
      const data = await resp.json();

      if (data.output) this.appendRepl(data.output.trim(), 'result');
      if (data.error) this.appendRepl(data.error.trim(), 'error');
      if (data.exit_code !== 0 && !data.error && !data.output) {
        this.appendRepl('(no output, exit code ' + data.exit_code + ')', 'info');
      }
    } catch (e) {
      this.appendRepl('Error: ' + e.message, 'error');
    }
    this.setStatus('ready', 'Ready');
  },

  async runProgram() {
    const textarea = this.getEditor();
    const source = textarea.value.trim();
    if (!source) {
      this.toast('No program to run', 'error');
      return;
    }

    this.setStatus('busy', 'Running...');
    this.switchBottomTab('output');
    const outputEl = document.getElementById('output-content');
    outputEl.textContent = 'Running specification...\n';

    try {
      const resp = await fetch('/api/run', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ source }),
      });
      const data = await resp.json();

      let result = '';
      if (data.output) result += data.output;
      if (data.error) result += '\n--- stderr ---\n' + data.error;
      result += '\n[Exit code: ' + (data.exit_code ?? '?') + ']';
      outputEl.textContent = result;

      if (data.exit_code === 0) {
        this.toast('Program completed successfully', 'success');
      } else {
        this.toast('Program exited with errors', 'error');
      }
    } catch (e) {
      outputEl.textContent = 'Error: ' + e.message;
      this.toast('Failed to run program', 'error');
    }
    this.setStatus('ready', 'Ready');
  },

  // -----------------------------------------------------------------------
  // Quick Actions (normalize, check, solve, whatis)
  // -----------------------------------------------------------------------
  async quickAction(action, formula) {
    if (!formula) {
      const textarea = this.getEditor();
      if (textarea.selectionStart !== textarea.selectionEnd) {
        formula = textarea.value.substring(textarea.selectionStart, textarea.selectionEnd).trim();
      } else {
        this.toast('Select a formula first', 'info');
        return;
      }
    }

    this.setStatus('busy', action + '...');
    let endpoint, body;

    switch (action) {
      case 'normalize':
      case 'dnf':
      case 'cnf':
      case 'nnf':
      case 'pnf':
      case 'mnf':
      case 'anf':
        endpoint = '/api/normalize';
        body = { formula, form: action };
        break;
      case 'sat':
      case 'unsat':
      case 'valid':
        endpoint = '/api/check';
        body = { formula, type: action };
        break;
      case 'solve':
        endpoint = '/api/solve';
        body = { formula };
        break;
      case 'whatis':
        endpoint = '/api/parse';
        body = { formula };
        break;
      default:
        this.toast('Unknown action: ' + action, 'error');
        return;
    }

    try {
      const resp = await fetch(endpoint, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(body),
      });
      const data = await resp.json();

      this.switchBottomTab('repl');
      this.appendRepl(`${action} ${formula}`, 'cmd');
      if (data.output) this.appendRepl(data.output.trim(), 'result');
      if (data.error) this.appendRepl(data.error.trim(), 'error');
    } catch (e) {
      this.appendRepl('Error: ' + e.message, 'error');
    }
    this.setStatus('ready', 'Ready');
  },

  // -----------------------------------------------------------------------
  // Fragment selection
  // -----------------------------------------------------------------------
  async setFragment(frag) {
    this.appendRepl(`fragment ${frag}.`, 'cmd');
    try {
      const resp = await fetch('/api/eval', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ command: `fragment ${frag}.` }),
      });
      const data = await resp.json();
      if (data.output) this.appendRepl(data.output.trim(), 'result');
      this.toast(`Switched to ${frag} fragment`, 'info');
    } catch (e) {
      this.appendRepl('Error: ' + e.message, 'error');
    }
  },

  // -----------------------------------------------------------------------
  // REPL
  // -----------------------------------------------------------------------
  initRepl() {
    const input = document.getElementById('repl-input');
    input.addEventListener('keydown', (e) => {
      if (e.key === 'Enter') {
        e.preventDefault();
        const cmd = input.value.trim();
        if (!cmd) return;
        this.replHistory.push(cmd);
        this.replHistoryIdx = this.replHistory.length;
        input.value = '';
        this.evalReplCommand(cmd);
      } else if (e.key === 'ArrowUp') {
        e.preventDefault();
        if (this.replHistoryIdx > 0) {
          this.replHistoryIdx--;
          input.value = this.replHistory[this.replHistoryIdx];
        }
      } else if (e.key === 'ArrowDown') {
        e.preventDefault();
        if (this.replHistoryIdx < this.replHistory.length - 1) {
          this.replHistoryIdx++;
          input.value = this.replHistory[this.replHistoryIdx];
        } else {
          this.replHistoryIdx = this.replHistory.length;
          input.value = '';
        }
      } else if (e.key === 'l' && e.ctrlKey) {
        e.preventDefault();
        document.getElementById('repl-output').innerHTML = '';
      }
    });

    // Welcome message
    this.appendRepl('Tau-lang IDE REPL. Type commands ending with "." (e.g., "help.")', 'info');
  },

  async evalReplCommand(cmd) {
    // Add period if missing
    if (!cmd.endsWith('.') && !['q', 'quit', 'help', 'version', 'v', 'h'].includes(cmd.split(/\s/)[0])) {
      cmd += '.';
    }

    this.appendRepl(cmd, 'cmd');
    this.setStatus('busy', 'Evaluating...');

    try {
      const resp = await fetch('/api/eval', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ command: cmd }),
      });
      const data = await resp.json();

      if (data.output) this.appendRepl(data.output.trim(), 'result');
      if (data.error) this.appendRepl(data.error.trim(), 'error');
      if (data.exit_code !== 0 && !data.output && !data.error) {
        this.appendRepl('(exit code ' + data.exit_code + ')', 'info');
      }
    } catch (e) {
      this.appendRepl('Error: ' + e.message, 'error');
    }
    this.setStatus('ready', 'Ready');
  },

  appendRepl(text, type) {
    const output = document.getElementById('repl-output');
    const div = document.createElement('div');
    div.className = 'repl-line repl-' + type;
    if (type === 'cmd') {
      div.innerHTML = '<span class="repl-prompt">tau&gt;</span> ' + escHtml(text);
    } else {
      div.textContent = text;
    }
    output.appendChild(div);
    // Auto-scroll
    const container = output.closest('.bottom-pane');
    if (container) container.scrollTop = container.scrollHeight;
  },

  // -----------------------------------------------------------------------
  // Bottom Panel
  // -----------------------------------------------------------------------
  switchBottomTab(panel) {
    document.querySelectorAll('#bottom-tabs .btab').forEach(b => {
      b.classList.toggle('active', b.dataset.panel === panel);
    });
    document.querySelectorAll('.bottom-pane').forEach(p => {
      p.classList.toggle('active', p.id === 'panel-' + panel);
    });
  },

  toggleBottomPanel() {
    const panel = document.getElementById('bottom-panel');
    this.bottomVisible = !this.bottomVisible;
    panel.style.display = this.bottomVisible ? '' : 'none';
  },

  clearBottomPanel() {
    const active = document.querySelector('.bottom-pane.active');
    if (active) {
      if (active.id === 'panel-repl') {
        document.getElementById('repl-output').innerHTML = '';
      } else {
        const pre = active.querySelector('pre');
        if (pre) pre.textContent = '';
        const div = active.querySelector('div:not(.repl-input-container)');
        if (div) div.innerHTML = '';
      }
    }
  },

  // -----------------------------------------------------------------------
  // Bottom Panel Resizer
  // -----------------------------------------------------------------------
  initResizer() {
    const panel = document.getElementById('bottom-panel');
    const handle = document.createElement('div');
    handle.className = 'resize-handle';
    panel.prepend(handle);

    let startY, startH;
    const onMouseMove = (e) => {
      const delta = startY - e.clientY;
      panel.style.height = Math.max(100, Math.min(600, startH + delta)) + 'px';
    };
    const onMouseUp = () => {
      document.removeEventListener('mousemove', onMouseMove);
      document.removeEventListener('mouseup', onMouseUp);
    };
    handle.addEventListener('mousedown', (e) => {
      startY = e.clientY;
      startH = panel.offsetHeight;
      document.addEventListener('mousemove', onMouseMove);
      document.addEventListener('mouseup', onMouseUp);
    });
  },

  // -----------------------------------------------------------------------
  // Sidebar
  // -----------------------------------------------------------------------
  toggleSidebar() {
    const sidebar = document.getElementById('sidebar');
    this.sidebarVisible = !this.sidebarVisible;
    sidebar.style.display = this.sidebarVisible ? '' : 'none';
  },

  toggleSection(header) {
    header.classList.toggle('collapsed');
    const body = header.nextElementSibling;
    body.classList.toggle('hidden');
  },

  async loadExamples() {
    try {
      const resp = await fetch('/api/examples');
      const data = await resp.json();
      const tree = document.getElementById('examples-tree');
      tree.innerHTML = '';
      (data.examples || []).forEach(ex => {
        const item = document.createElement('div');
        item.className = 'tree-item';
        item.title = ex.name;
        // Show shorter display name — strip common prefix for readability
        const displayName = ex.name.replace(/^test_integration-/, '');
        item.innerHTML = `<span class="icon">&#128196;</span>${escHtml(displayName)}`;
        item.onclick = () => this.loadExampleFile(ex);
        tree.appendChild(item);
      });
      if (!data.examples || data.examples.length === 0) {
        tree.innerHTML = '<div class="tree-item" style="color:var(--overlay0)">No examples found</div>';
      }
    } catch (e) {
      console.error('Failed to load examples:', e);
    }
  },

  async loadExampleFile(ex) {
    // For test files, we'd need a file-read endpoint
    this.toast(`Loading ${ex.name}...`, 'info');
  },

  loadTemplates() {
    const tree = document.getElementById('templates-tree');
    const templates = [
      { name: 'Empty Specification', content: '# Tau specification\n\n# Define I/O streams\ni1 := in console.\no1 := out console.\n\n# Specification\nalways o1[t] = i1[t].\n' },
      { name: 'Safety Property', content: '# Safety property: output is never zero\ni1 := in console.\no1 := out console.\n\nalways o1[t] != 0.\n' },
      { name: 'Liveness Property', content: '# Liveness: every request gets a response\ni1 := in console.\no1 := out console.\n\nalways (i1[t] = 1 -> sometimes o1[t] = 1).\n' },
      { name: 'Boolean Formula', content: '# Boolean formula exploration\n# Try: sat, valid, solve, dnf, cnf, nnf\n\nsat ex x (x = {01}).\n' },
      { name: 'Recurrence Relation', content: '# Define and use a recurrence relation\nf(x) := x | x\'.\nsat f({01}) = 1.\n' },
      { name: 'CTL* Fragment', content: '# CTL* operators (A, E, semantic negation)\n# First switch to CTL* fragment:\nfragment ctl_star.\n\n# Forall-paths eventually safe\n# A F (o1[t] = 1).\n' },
      { name: 'Quantifier Elimination', content: '# Quantifier elimination example\nqelim ex x (x & y = x).\n' },
      { name: 'Substitution', content: '# Substitution example\nsubst (a && b) [a / (c || d)].\n' },
    ];

    tree.innerHTML = '';
    templates.forEach(t => {
      const item = document.createElement('div');
      item.className = 'tree-item';
      item.innerHTML = `<span class="icon">&#128221;</span>${escHtml(t.name)}`;
      item.onclick = () => {
        const textarea = this.getEditor();
        if (textarea) {
          textarea.value = t.content;
          this.onEditorChange();
          this.toast(`Loaded template: ${t.name}`, 'info');
        }
      };
      tree.appendChild(item);
    });
  },

  // -----------------------------------------------------------------------
  // Help System
  // -----------------------------------------------------------------------
  async loadHelp() {
    try {
      const resp = await fetch('/api/help');
      this.helpData = await resp.json();
      this.buildHelpTOC();
    } catch (e) {
      console.error('Failed to load help:', e);
    }
  },

  buildHelpTOC() {
    const toc = document.getElementById('help-toc');
    if (!this.helpData) return;
    toc.innerHTML = '';
    const topics = this.helpData.topics || {};
    for (const [key, val] of Object.entries(topics)) {
      const item = document.createElement('div');
      item.className = 'help-toc-item';
      item.textContent = val.title || key;
      item.dataset.topic = key;
      item.onclick = () => this.showHelpTopic(key);
      toc.appendChild(item);
    }
    // Add grammar reference entry
    const gramItem = document.createElement('div');
    gramItem.className = 'help-toc-item';
    gramItem.textContent = 'Grammar Reference';
    gramItem.dataset.topic = 'grammar-ref';
    gramItem.onclick = () => this.showHelpTopic('grammar-ref');
    toc.appendChild(gramItem);
  },

  showHelpTopic(topic) {
    this.currentHelpTopic = topic;
    const body = document.getElementById('help-body');

    // Special: grammar reference (rendered from grammar data)
    if (topic === 'grammar-ref') {
      body.innerHTML = this.renderGrammarReference();
      document.querySelectorAll('.help-toc-item').forEach(i => {
        i.classList.toggle('active', i.dataset.topic === topic);
      });
      return;
    }

    if (!this.helpData) return;
    const topics = this.helpData.topics || {};
    const data = topics[topic];
    if (!data) return;

    // Render markdown
    if (typeof marked !== 'undefined') {
      body.innerHTML = marked.parse(data.content || '');
    } else {
      body.innerHTML = `<pre>${escHtml(data.content || '')}</pre>`;
    }

    // Update active TOC item
    document.querySelectorAll('.help-toc-item').forEach(i => {
      i.classList.toggle('active', i.dataset.topic === topic);
    });
  },

  toggleHelp() {
    const panel = document.getElementById('right-panel');
    const visible = panel.style.display !== 'none';
    panel.style.display = visible ? 'none' : '';
    if (!visible && this.helpData) {
      // Show getting-started by default
      if (!this.currentHelpTopic) {
        this.showHelpTopic('getting-started');
      }
    }
  },

  searchHelp(query) {
    if (!this.helpData) return;
    const q = query.toLowerCase();
    document.querySelectorAll('.help-toc-item').forEach(item => {
      const topic = item.dataset.topic;
      const data = this.helpData.topics[topic];
      const matches = !q
        || (data.title && data.title.toLowerCase().includes(q))
        || (data.content && data.content.toLowerCase().includes(q));
      item.style.display = matches ? '' : 'none';
    });
  },

  // -----------------------------------------------------------------------
  // Command Palette
  // -----------------------------------------------------------------------
  initCommands() {
    this.commands = [
      { name: 'New File', shortcut: 'Ctrl+N', action: () => this.newFile() },
      { name: 'Open File', shortcut: 'Ctrl+O', action: () => this.openFile() },
      { name: 'Save File', shortcut: 'Ctrl+S', action: () => this.saveFile() },
      { name: 'Run Program', shortcut: 'Shift+Enter', action: () => this.runProgram() },
      { name: 'Evaluate Selection', shortcut: 'Ctrl+Enter', action: () => this.evalSelection() },
      { name: 'Toggle Help', shortcut: 'F1', action: () => this.toggleHelp() },
      { name: 'Toggle Sidebar', shortcut: '', action: () => this.toggleSidebar() },
      { name: 'Toggle Bottom Panel', shortcut: '', action: () => this.toggleBottomPanel() },
      { name: 'Clear Output', shortcut: '', action: () => this.clearBottomPanel() },
      { name: 'Check SAT', shortcut: '', action: () => this.quickAction('sat') },
      { name: 'Check Valid', shortcut: '', action: () => this.quickAction('valid') },
      { name: 'Check UNSAT', shortcut: '', action: () => this.quickAction('unsat') },
      { name: 'Normalize', shortcut: '', action: () => this.quickAction('normalize') },
      { name: 'Convert to DNF', shortcut: '', action: () => this.quickAction('dnf') },
      { name: 'Convert to CNF', shortcut: '', action: () => this.quickAction('cnf') },
      { name: 'Convert to NNF', shortcut: '', action: () => this.quickAction('nnf') },
      { name: 'Solve', shortcut: '', action: () => this.quickAction('solve') },
      { name: 'What Is (type inspection)', shortcut: '', action: () => this.quickAction('whatis') },
      { name: 'Fragment: LTL', shortcut: '', action: () => { document.getElementById('fragment-select').value = 'ltl'; this.setFragment('ltl'); }},
      { name: 'Fragment: CTL*', shortcut: '', action: () => { document.getElementById('fragment-select').value = 'ctl_star'; this.setFragment('ctl_star'); }},
      { name: 'Show REPL', shortcut: '', action: () => this.switchBottomTab('repl') },
      { name: 'Show Output', shortcut: '', action: () => this.switchBottomTab('output') },
      { name: 'Show Problems', shortcut: '', action: () => this.switchBottomTab('problems') },
      { name: 'Show AST', shortcut: '', action: () => this.switchBottomTab('ast') },
      { name: 'Visualize Formula', shortcut: 'Ctrl+Shift+V', action: () => this.visualize() },
      { name: 'AI Assistant', shortcut: '', action: () => this.toggleAI() },
      { name: 'AI: Generate from Natural Language', shortcut: '', action: () => { document.getElementById('ai-mode').value = 'generate'; this.toggleAI(); }},
      { name: 'AI: Explain Code', shortcut: '', action: () => { document.getElementById('ai-mode').value = 'explain'; this.toggleAI(); this.sendAI(); }},
      { name: 'AI: Set API Key', shortcut: '', action: () => this.showAPIKeyDialog() },
      { name: 'Show Grammar Reference', shortcut: '', action: () => { this.showHelpTopic('grammar-ref'); this.toggleHelp(); }},
    ];
  },

  showCommandPalette() {
    const palette = document.getElementById('command-palette');
    palette.style.display = '';
    const input = document.getElementById('palette-input');
    input.value = '';
    input.focus();
    this.filterCommands('');

    input.onkeydown = (e) => {
      if (e.key === 'Escape') {
        this.hideCommandPalette();
      } else if (e.key === 'Enter') {
        const selected = document.querySelector('.palette-item.selected');
        if (selected) selected.click();
        this.hideCommandPalette();
      } else if (e.key === 'ArrowDown' || e.key === 'ArrowUp') {
        e.preventDefault();
        const items = document.querySelectorAll('.palette-item');
        let idx = -1;
        items.forEach((item, i) => { if (item.classList.contains('selected')) idx = i; });
        items.forEach(item => item.classList.remove('selected'));
        if (e.key === 'ArrowDown') idx = Math.min(idx + 1, items.length - 1);
        else idx = Math.max(idx - 1, 0);
        if (items[idx]) items[idx].classList.add('selected');
      }
    };
  },

  hideCommandPalette() {
    document.getElementById('command-palette').style.display = 'none';
    const textarea = this.getEditor();
    if (textarea) textarea.focus();
  },

  filterCommands(query) {
    const q = query.toLowerCase();
    const results = document.getElementById('palette-results');
    results.innerHTML = '';
    const filtered = this.commands.filter(c => c.name.toLowerCase().includes(q));
    filtered.forEach((cmd, i) => {
      const item = document.createElement('div');
      item.className = 'palette-item' + (i === 0 ? ' selected' : '');
      item.innerHTML = `
        <span>${escHtml(cmd.name)}</span>
        <span class="shortcut">${escHtml(cmd.shortcut)}</span>
      `;
      item.onclick = () => {
        cmd.action();
        this.hideCommandPalette();
      };
      results.appendChild(item);
    });
  },

  // -----------------------------------------------------------------------
  // Keyboard Shortcuts
  // -----------------------------------------------------------------------
  initKeyboard() {
    document.addEventListener('keydown', (e) => {
      // Ctrl+Shift+V — visualize
      if (e.key === 'V' && e.ctrlKey && e.shiftKey) {
        e.preventDefault();
        this.visualize();
        return;
      }
      // Ctrl+Shift+P — command palette
      if (e.key === 'P' && e.ctrlKey && e.shiftKey) {
        e.preventDefault();
        this.showCommandPalette();
        return;
      }
      // F1 — help
      if (e.key === 'F1') {
        e.preventDefault();
        this.toggleHelp();
        return;
      }
      // Ctrl+N — new file
      if (e.key === 'n' && e.ctrlKey && !e.shiftKey) {
        e.preventDefault();
        this.newFile();
        return;
      }
      // Ctrl+O — open
      if (e.key === 'o' && e.ctrlKey) {
        e.preventDefault();
        this.openFile();
        return;
      }
      // Ctrl+S — save
      if (e.key === 's' && e.ctrlKey) {
        e.preventDefault();
        this.saveFile();
        return;
      }
      // Ctrl+G — go to line
      if (e.key === 'g' && e.ctrlKey) {
        e.preventDefault();
        this.goToLine();
        return;
      }
      // Escape — close panels
      if (e.key === 'Escape') {
        this.hideCommandPalette();
        return;
      }
    });
  },

  goToLine() {
    const line = prompt('Go to line:');
    if (!line) return;
    const num = parseInt(line);
    if (isNaN(num)) return;
    const textarea = this.getEditor();
    if (!textarea) return;
    const lines = textarea.value.split('\n');
    let pos = 0;
    for (let i = 0; i < Math.min(num - 1, lines.length); i++) {
      pos += lines[i].length + 1;
    }
    textarea.selectionStart = textarea.selectionEnd = pos;
    textarea.focus();
    this.updateCursorPos();
  },

  // -----------------------------------------------------------------------
  // Status & Toast
  // -----------------------------------------------------------------------
  async checkStatus() {
    try {
      const resp = await fetch('/api/status');
      const data = await resp.json();
      if (!data.tau_available) {
        this.setStatus('error', 'tau binary not found');
        this.toast('tau binary not found. Build it with: cmake --build build --target tau', 'error');
      }
    } catch (e) {
      this.setStatus('error', 'Server error');
    }
  },

  setStatus(type, text) {
    const el = document.getElementById('status-indicator');
    el.className = 'status-' + type;
    el.textContent = text;
  },

  toast(message, type = 'info') {
    const container = document.getElementById('toast-container');
    const toast = document.createElement('div');
    toast.className = 'toast ' + type;
    toast.textContent = message;
    container.appendChild(toast);
    setTimeout(() => toast.remove(), 4000);
  },

  // -----------------------------------------------------------------------
  // Grammar-aware tooltip (hover in overlay)
  // -----------------------------------------------------------------------
  initTooltip() {
    this._tooltip = document.createElement('div');
    this._tooltip.className = 'token-tooltip';
    this._tooltip.style.display = 'none';
    document.body.appendChild(this._tooltip);

    const overlay = document.getElementById('highlight-overlay');
    if (!overlay) return;

    overlay.addEventListener('mousemove', (e) => {
      const target = e.target.closest('span[class^="hl-"]');
      if (!target) { this._tooltip.style.display = 'none'; return; }
      const word = target.textContent;
      const info = TauSyntax.lookupToken(word);
      if (!info) { this._tooltip.style.display = 'none'; return; }
      this._tooltip.innerHTML =
        `<div class="tt-syntax">${escHtml(info.syntax || word)}</div>` +
        `<div class="tt-doc">${escHtml(info.doc || '')}</div>` +
        `<div class="tt-cat">${escHtml(info.category || '')}</div>`;
      this._tooltip.style.display = '';
      this._tooltip.style.left = e.pageX + 12 + 'px';
      this._tooltip.style.top  = e.pageY + 12 + 'px';
    });

    overlay.addEventListener('mouseleave', () => {
      this._tooltip.style.display = 'none';
    });
  },

  // -----------------------------------------------------------------------
  // Temporal formula visualization
  // -----------------------------------------------------------------------
  visualize() {
    const textarea = this.getEditor();
    if (!textarea) return;
    // Use selection if available, else entire editor content
    let formula = '';
    if (textarea.selectionStart !== textarea.selectionEnd) {
      formula = textarea.value.slice(textarea.selectionStart, textarea.selectionEnd);
    } else {
      formula = textarea.value;
    }
    const el = document.getElementById('viz-content');
    if (el && typeof TauViz !== 'undefined') {
      el.innerHTML = TauViz.renderVisualization(formula);
    }
    this.switchBottomTab('viz');
  },

  // -----------------------------------------------------------------------
  // AI Assistant (DeepSeek)
  // -----------------------------------------------------------------------
  _getAPIKey() {
    return localStorage.getItem('tau_ide_deepseek_key') || '';
  },

  showAPIKeyDialog() {
    const dialog = document.getElementById('api-key-dialog');
    const input = document.getElementById('api-key-input');
    input.value = this._getAPIKey();
    dialog.style.display = '';
    input.focus();
  },

  hideAPIKeyDialog() {
    document.getElementById('api-key-dialog').style.display = 'none';
  },

  saveAPIKey() {
    const key = document.getElementById('api-key-input').value.trim();
    if (key) {
      localStorage.setItem('tau_ide_deepseek_key', key);
      this.toast('API key saved (browser only)', 'info');
    } else {
      localStorage.removeItem('tau_ide_deepseek_key');
      this.toast('API key removed', 'info');
    }
    this.hideAPIKeyDialog();
  },

  toggleAI() {
    this.switchBottomTab('ai');
    if (!this.bottomVisible) this.toggleBottomPanel();
    document.getElementById('ai-input').focus();
  },

  async sendAI() {
    const apiKey = this._getAPIKey();
    if (!apiKey) {
      this.showAPIKeyDialog();
      this.toast('Please set your DeepSeek API key first', 'error');
      return;
    }

    const mode = document.getElementById('ai-mode').value;
    const model = document.getElementById('ai-model').value;
    const input = document.getElementById('ai-input');
    const output = document.getElementById('ai-output');
    const status = document.getElementById('ai-status');
    const textarea = this.getEditor();
    const prompt = input.value.trim();

    if (mode === 'explain') {
      // Use selected text or full editor content
      const code = textarea && textarea.selectionStart !== textarea.selectionEnd
        ? textarea.value.slice(textarea.selectionStart, textarea.selectionEnd)
        : (textarea ? textarea.value : '');
      if (!code.trim()) {
        this.toast('Select code to explain or write some code first', 'error');
        return;
      }
      status.textContent = 'Thinking...';
      output.innerHTML = '<div style="color:#6c7086;">Sending to DeepSeek...</div>';
      try {
        const resp = await fetch('/api/llm/explain', {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({ api_key: apiKey, code, model }),
        });
        const data = await resp.json();
        if (data.error) {
          output.innerHTML = `<div style="color:#f38ba8;">Error: ${escHtml(data.error)}</div>`;
          status.textContent = 'Error';
        } else {
          const html = typeof marked !== 'undefined' ? marked.parse(data.explanation || '') : `<pre>${escHtml(data.explanation || '')}</pre>`;
          let result = html;
          if (data.reasoning) {
            result += `<details style="margin-top:12px;"><summary style="color:#6c7086;cursor:pointer;">Show reasoning</summary><pre style="font-size:11px;color:#a6adc8;white-space:pre-wrap;margin-top:8px;">${escHtml(data.reasoning)}</pre></details>`;
          }
          if (data.usage) {
            result += `<div style="color:#6c7086;font-size:10px;margin-top:8px;">Model: ${escHtml(data.model || model)} | Tokens: ${data.usage.total_tokens || '?'}</div>`;
          }
          output.innerHTML = result;
          status.textContent = 'Done';
        }
      } catch (e) {
        output.innerHTML = `<div style="color:#f38ba8;">Network error: ${escHtml(e.message)}</div>`;
        status.textContent = 'Error';
      }
      return;
    }

    if (!prompt) {
      this.toast('Enter a prompt', 'error');
      return;
    }

    status.textContent = 'Thinking...';
    output.innerHTML = '<div style="color:#6c7086;">Sending to DeepSeek...</div>';
    input.value = '';

    try {
      let endpoint, body;
      if (mode === 'generate') {
        endpoint = '/api/llm/generate';
        body = {
          api_key: apiKey,
          prompt,
          context: textarea ? textarea.value : '',
          model,
        };
      } else {
        // assist (context-aware)
        endpoint = '/api/llm/assist';
        const cursorLine = textarea ? textarea.value.slice(0, textarea.selectionStart).split('\n').length : 0;
        body = {
          api_key: apiKey,
          question: prompt,
          editor_content: textarea ? textarea.value : '',
          cursor_line: cursorLine,
          model,
        };
      }

      const resp = await fetch(endpoint, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(body),
      });
      const data = await resp.json();
      if (data.error) {
        output.innerHTML = `<div style="color:#f38ba8;">Error: ${escHtml(data.error)}</div>`;
        status.textContent = 'Error';
        return;
      }

      let html = '';
      if (mode === 'generate') {
        // Show generated code with insert button
        const code = data.code || data.raw_response || '';
        html += `<div style="margin-bottom:8px;">
          <button class="tb-btn" onclick="IDE.insertAICode()" style="font-size:11px;margin-bottom:6px;">Insert into editor</button>
          <button class="tb-btn" onclick="IDE.replaceWithAICode()" style="font-size:11px;margin-bottom:6px;">Replace editor content</button>
        </div>`;
        html += `<pre id="ai-generated-code" style="background:#181825;padding:8px;border-radius:4px;white-space:pre-wrap;border:1px solid #45475a;">${escHtml(code)}</pre>`;
        if (data.reasoning) {
          html += `<details style="margin-top:12px;"><summary style="color:#6c7086;cursor:pointer;">Show reasoning</summary><pre style="font-size:11px;color:#a6adc8;white-space:pre-wrap;margin-top:8px;">${escHtml(data.reasoning)}</pre></details>`;
        }
      } else {
        // assist mode — show answer with code snippets
        const answer = data.answer || '';
        html = typeof marked !== 'undefined' ? marked.parse(answer) : `<pre>${escHtml(answer)}</pre>`;
        if (data.code_snippets && data.code_snippets.length > 0) {
          html += '<div style="margin-top:12px;">';
          data.code_snippets.forEach((snippet, idx) => {
            html += `<div style="margin-bottom:8px;">
              <button class="tb-btn" onclick="IDE.insertSnippet(${idx})" style="font-size:11px;">Insert snippet ${idx + 1}</button>
              <pre class="ai-snippet" style="background:#181825;padding:8px;border-radius:4px;white-space:pre-wrap;margin-top:4px;border:1px solid #45475a;">${escHtml(snippet)}</pre>
            </div>`;
          });
          html += '</div>';
          this._aiSnippets = data.code_snippets;
        }
        if (data.reasoning) {
          html += `<details style="margin-top:12px;"><summary style="color:#6c7086;cursor:pointer;">Show reasoning</summary><pre style="font-size:11px;color:#a6adc8;white-space:pre-wrap;margin-top:8px;">${escHtml(data.reasoning)}</pre></details>`;
        }
      }

      if (data.usage) {
        html += `<div style="color:#6c7086;font-size:10px;margin-top:8px;">Model: ${escHtml(data.model || model)} | Tokens: ${data.usage.total_tokens || '?'}</div>`;
      }

      output.innerHTML = html;
      status.textContent = 'Done';
    } catch (e) {
      output.innerHTML = `<div style="color:#f38ba8;">Network error: ${escHtml(e.message)}</div>`;
      status.textContent = 'Error';
    }
  },

  insertAICode() {
    const code = document.getElementById('ai-generated-code');
    if (!code) return;
    const textarea = this.getEditor();
    if (!textarea) return;
    const pos = textarea.selectionStart;
    const before = textarea.value.slice(0, pos);
    const after = textarea.value.slice(pos);
    textarea.value = before + code.textContent + after;
    this.onEditorChange();
    this.toast('Code inserted at cursor', 'info');
  },

  replaceWithAICode() {
    const code = document.getElementById('ai-generated-code');
    if (!code) return;
    const textarea = this.getEditor();
    if (!textarea) return;
    textarea.value = code.textContent;
    this.onEditorChange();
    this.toast('Editor content replaced', 'info');
  },

  _aiSnippets: [],

  insertSnippet(idx) {
    if (idx >= this._aiSnippets.length) return;
    const textarea = this.getEditor();
    if (!textarea) return;
    const pos = textarea.selectionStart;
    const before = textarea.value.slice(0, pos);
    const after = textarea.value.slice(pos);
    textarea.value = before + '\n' + this._aiSnippets[idx] + '\n' + after;
    this.onEditorChange();
    this.toast(`Snippet ${idx + 1} inserted`, 'info');
  },

  // -----------------------------------------------------------------------
  // Grammar Reference (rendered in help panel)
  // -----------------------------------------------------------------------
  renderGrammarReference() {
    const ref = TauSyntax.getGrammarReference();
    if (!ref) return '<p>Grammar data not loaded.</p>';

    let html = '<h2>Grammar Reference</h2>';
    html += '<p style="color:#a6adc8;font-size:12px;">Derived from <code>parser/tau.tgf</code></p>';

    html += '<h3>Well-Formed Formulas (wff)</h3>';
    html += this._grammarTable(ref.wff, 'wff');
    html += '<h3>Boolean Functions (bf)</h3>';
    html += this._grammarTable(ref.bf, 'bf');
    html += '<h3>REPL Commands</h3>';
    html += this._grammarTable(ref.commands, 'cmd');
    html += '<h3>Constant-Time Constraints</h3>';
    html += this._grammarTable(ref.constraints, 'ctn');

    html += '<h3>WFF Operators</h3>';
    html += '<table class="grammar-table"><tr><th>Op</th><th>Name</th><th>Label</th></tr>';
    for (const [op, name, label] of (ref.wff_ops || [])) {
      html += `<tr><td class="grammar-syntax">${escHtml(op)}</td><td class="grammar-doc">${escHtml(name)}</td><td class="grammar-label">${escHtml(label)}</td></tr>`;
    }
    html += '</table>';

    html += '<h3>BF Operators</h3>';
    html += '<table class="grammar-table"><tr><th>Op</th><th>Name</th><th>Label</th></tr>';
    for (const [op, name, label] of (ref.bf_ops || [])) {
      html += `<tr><td class="grammar-syntax">${escHtml(op)}</td><td class="grammar-doc">${escHtml(name)}</td><td class="grammar-label">${escHtml(label)}</td></tr>`;
    }
    html += '</table>';

    html += '<h3>Types</h3><ul>';
    for (const t of (ref.types || [])) { html += `<li><code>${escHtml(t)}</code></li>`; }
    html += '</ul>';

    html += '<h3>Fragments</h3><ul>';
    for (const f of (ref.fragments || [])) { html += `<li><code>${escHtml(f)}</code></li>`; }
    html += '</ul>';

    const raw = TauSyntax.getRawGrammar();
    if (raw) {
      html += '<h3>Raw Grammar (tau.tgf)</h3>';
      html += `<pre style="font-size:11px;max-height:400px;overflow:auto;background:#1e1e2e;padding:8px;border-radius:4px;">${escHtml(raw)}</pre>`;
    }
    return html;
  },

  _grammarTable(rules) {
    if (!rules || !rules.length) return '';
    let html = '<table class="grammar-table"><tr><th>Syntax</th><th>Description</th><th>Label</th></tr>';
    for (const rule of rules) {
      html += `<tr><td class="grammar-syntax">${escHtml(rule.syntax || '')}</td><td class="grammar-doc">${escHtml(rule.doc || '')}</td><td class="grammar-label">${escHtml(rule.label || '')}</td></tr>`;
    }
    return html + '</table>';
  },
};

// Utility
function escHtml(s) {
  const div = document.createElement('div');
  div.textContent = s;
  return div.innerHTML;
}

// Boot
document.addEventListener('DOMContentLoaded', () => IDE.init());
