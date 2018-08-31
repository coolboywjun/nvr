object Form1: TForm1
  Left = 369
  Top = 338
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'NVR'#25209#37327#21319#32423#24037#20855'v3.0--'#20339#20449#25463
  ClientHeight = 392
  ClientWidth = 661
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object lblInfo: TLabel
    Left = 240
    Top = 280
    Width = 409
    Height = 33
    AutoSize = False
    Font.Charset = ANSI_CHARSET
    Font.Color = clRed
    Font.Height = -16
    Font.Name = #23435#20307
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lblClientNum: TLabel
    Left = 240
    Top = 224
    Width = 201
    Height = 33
    AutoSize = False
    Font.Charset = ANSI_CHARSET
    Font.Color = clRed
    Font.Height = -16
    Font.Name = #23435#20307
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lblConfig: TLabel
    Left = 448
    Top = 224
    Width = 201
    Height = 33
    AutoSize = False
    Font.Charset = ANSI_CHARSET
    Font.Color = clRed
    Font.Height = -16
    Font.Name = #23435#20307
    Font.Style = [fsBold]
    ParentFont = False
  end
  object GroupBox1: TGroupBox
    Left = 240
    Top = 16
    Width = 409
    Height = 89
    Caption = #36873#25321#21319#32423#25991#20214
    TabOrder = 2
    object Label1: TLabel
      Left = 8
      Top = 24
      Width = 65
      Height = 25
      AutoSize = False
      Caption = #21319#32423#25991#20214
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = #23435#20307
      Font.Style = []
      ParentFont = False
    end
    object Label10: TLabel
      Left = 8
      Top = 48
      Width = 73
      Height = 25
      AutoSize = False
      Caption = #37197#32622#25991#20214
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = #23435#20307
      Font.Style = []
      ParentFont = False
    end
    object editUpdateFile: TEdit
      Left = 80
      Top = 16
      Width = 209
      Height = 25
      TabStop = False
      AutoSize = False
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = #23435#20307
      Font.Style = []
      ParentFont = False
      TabOrder = 1
    end
    object btnUpdateFile: TButton
      Left = 296
      Top = 16
      Width = 105
      Height = 25
      Caption = #36873#25321#21319#32423#25991#20214'...'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = #23435#20307
      Font.Style = []
      ParentFont = False
      TabOrder = 0
      OnClick = btnUpdateFileClick
    end
    object editCfg: TEdit
      Left = 80
      Top = 48
      Width = 209
      Height = 25
      TabStop = False
      AutoSize = False
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = #23435#20307
      Font.Style = []
      ParentFont = False
      TabOrder = 2
    end
    object btnCfg: TButton
      Left = 296
      Top = 48
      Width = 105
      Height = 25
      Caption = #36873#25321#37197#32622#25991#20214
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = #23435#20307
      Font.Style = []
      ParentFont = False
      TabOrder = 3
      OnClick = btnCfgClick
    end
  end
  object btnUpdate: TButton
    Left = 400
    Top = 120
    Width = 241
    Height = 65
    Caption = #24320#22987#25209#37327#21319#32423
    Default = True
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -21
    Font.Name = #23435#20307
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 0
    OnClick = btnUpdateClick
  end
  object GroupBox2: TGroupBox
    Left = 16
    Top = 8
    Width = 217
    Height = 377
    Caption = #21319#32423#36827#24230
    TabOrder = 3
    object Label3: TLabel
      Left = 176
      Top = 184
      Width = 3
      Height = 13
    end
  end
  object GroupBox3: TGroupBox
    Left = 240
    Top = 120
    Width = 153
    Height = 65
    TabOrder = 1
    object Label4: TLabel
      Left = 8
      Top = 24
      Width = 65
      Height = 17
      AutoSize = False
      Caption = #35774#22791#25968#37327
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object ComboBox1: TComboBox
      Left = 80
      Top = 16
      Width = 57
      Height = 37
      Style = csDropDownList
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -24
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ImeMode = imDisable
      ItemHeight = 29
      ItemIndex = 3
      ParentFont = False
      TabOrder = 0
      Text = '4'
      Items.Strings = (
        '1'
        '2'
        '3'
        '4'
        '5'
        '6'
        '7'
        '8'
        '9'
        '10'
        '11'
        '12'
        '13'
        '14'
        '15'
        '16')
    end
  end
  object btnHelp: TButton
    Left = 240
    Top = 344
    Width = 75
    Height = 25
    Caption = #20351#29992#35828#26126
    TabOrder = 4
    OnClick = btnHelpClick
  end
  object openDlg: TOpenDialog
    FileName = 'kernel'
    Filter = #25152#26377#25991#20214'(*.*)|*.*'
    Left = 624
    Top = 360
  end
  object TimerTCP: TTimer
    Enabled = False
    Interval = 120000
    OnTimer = TimerTCPTimer
    Left = 584
    Top = 360
  end
end
