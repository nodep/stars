Option Explicit

Sub t()

    Const ChunkWidth As Long = 800
    Const ChunkHeight As Long = 800
    
    Dim app As Photoshop.Application
    
    Set app = GetObject(, "Photoshop.Application")
    
    app.Preferences.TypeUnits = psTypePixels
    app.Preferences.RulerUnits = psPixels
    app.DisplayDialogs = psDisplayNoDialogs

    Dim doc As Photoshop.Document
    Set doc = app.Open("c:\t100.png")
    
    Dim w As Long, h As Long
    
    h = doc.Height
    w = doc.Width
    
    Dim newDoc As Photoshop.Document, pngOptions As New Photoshop.PNGSaveOptions
    Dim x As Long, y As Long, docName As String
    pngOptions.Interlaced = False
    For x = 0 To w Step ChunkWidth
        For y = 0 To h Step ChunkHeight
    
            ' select part of the image
            doc.Selection.Select Array(Array(x, y), Array(x + ChunkWidth, y), Array(x + ChunkWidth, y + ChunkHeight), Array(x, y + ChunkHeight)), psReplaceSelection
            doc.Selection.Copy
            
            docName = "t" & x & "x" & y & ".png"
            Set newDoc = Documents.Add(ChunkWidth, ChunkHeight, doc.Resolution, docName)
            newDoc.Paste
            newDoc.SaveAs "c:\" & docName, pngOptions, True
            newDoc.Close
            
        Next
    Next
    
End Sub
