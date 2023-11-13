Imports System.Globalization
Imports System.IO.Ports

Public Class Form1

    Dim serialPort1 As New SerialPort("COM3", 9600)


    Private Sub Form1_Load(sender As Object, e As EventArgs) Handles MyBase.Load
        DisplayCurrentDate()

        Try
            serialPort1.Open()
            StartSerialReading()
        Catch ex As Exception
            If Not Me.IsDisposed AndAlso Not Me.Disposing Then
                MessageBox.Show("Insert your Arduino Uno Board: COM Port Not Found", "Error opening Windows Form", MessageBoxButtons.OK, MessageBoxIcon.Error)
                Me.Close()
            End If
        End Try
    End Sub

    Private Sub StartSerialReading()
        Dim serialReadingThread As New Threading.Thread(AddressOf ReadSerialData)

        serialReadingThread.Start()
    End Sub

    Private Sub ReadSerialData()
        While True
            Try
                Dim serialData As String = serialPort1.ReadLine()

                Invoke(Sub() UpdateLabel(serialData))
            Catch ex As Exception
                If serialPort1.IsOpen Then
                    MessageBox.Show("Error Reading Serial Port", "Error opening Serial Port", MessageBoxButtons.OK, MessageBoxIcon.Warning)
                End If
            End Try
        End While
    End Sub

    Private Sub UpdateLabel(serialData As String)
        Console.WriteLine($"Received Serial Data: {serialData}")

        Dim lines As String() = serialData.Split({vbLf, vbCr}, StringSplitOptions.RemoveEmptyEntries)

        For Each line As String In lines
            If line.StartsWith("pH Value:") Then

                Dim pHValue As String = line.Substring("pH Value:".Length).Trim()
                Console.WriteLine($"Extracted pH Value: {pHValue}")

                Invoke(Sub()
                           labelpH.Text = $"{pHValue}"
                       End Sub)
            ElseIf line.StartsWith("Water Condition:") Then

                Dim waterCondition As String = line.Substring("Water Condition:".Length).Trim()
                Console.WriteLine($"Extracted Water Condition: {waterCondition}")


                Invoke(Sub()
                           labelWaterCondition.Text = $"{waterCondition}"

                           If waterCondition = "Neutral" Then
                               labelWaterCondition.ForeColor = Color.Green
                           ElseIf waterCondition = "Acid" Then
                               labelWaterCondition.ForeColor = Color.Yellow
                           ElseIf waterCondition = "Basic" Then
                               labelWaterCondition.ForeColor = Color.Orange
                           End If
                       End Sub)

            ElseIf line.StartsWith("Pump Refill:") Then

                Dim pumpRefill As String = line.Substring("Pump Refill:".Length).Trim()
                Console.WriteLine($"Pump Refill: {pumpRefill}")

                Invoke(Sub()
                           Dim refillValue = $"{pumpRefill}"

                           If refillValue = 1 Then
                               labelRefill.Text = "Refilling Pump"
                               labelRefill.ForeColor = Color.Green
                           Else
                               labelRefill.Text = "Pump Off"
                               labelRefill.ForeColor = Color.Blue
                           End If
                       End Sub)

            ElseIf line.StartsWith("Pump Drain:") Then

                Dim pumpDrain As String = line.Substring("Pump Drain:".Length).Trim()
                Console.WriteLine($"Pump Drain: {pumpDrain}")

                Invoke(Sub()
                           Dim drainValue = $"{pumpDrain}"

                           If drainValue = 1 Then
                               labelDrain.Text = "Draining Pump"
                               labelDrain.ForeColor = Color.Green
                           Else
                               labelDrain.Text = "Pump Off"
                               labelDrain.ForeColor = Color.Blue
                           End If
                       End Sub)

            ElseIf line.StartsWith("Water Level:") Then

                Dim waterLevel As String = line.Substring("Water Level:".Length).Trim()
                Dim cultureInfo As New Globalization.CultureInfo("en-US")
                Dim textInfo As TextInfo = cultureInfo.TextInfo
                waterLevel = textInfo.ToTitleCase(waterLevel.ToLower())


                Console.WriteLine($"Water Level: {waterLevel}")

                Invoke(Sub()

                           labelWaterLevel.Text = $"Water Level {waterLevel}"

                           If waterLevel = "HIGH" Then
                               labelWaterLevel.ForeColor = Color.Green

                           ElseIf waterLevel = "LOW" Then
                               labelWaterLevel.ForeColor = Color.Yellow
                           Else
                               labelWaterLevel.ForeColor = Color.Orange
                           End If

                       End Sub)

            End If

        Next
    End Sub

    Private Function GetSubString(input As String, startMarker As String, endMarker As String) As String
        Dim startIndex As Integer = input.IndexOf(startMarker)
        If startIndex >= 0 Then
            startIndex += startMarker.Length
            Dim endIndex As Integer = input.IndexOf(endMarker, startIndex)
            If endIndex >= 0 Then
                Return input.Substring(startIndex, endIndex - startIndex).Trim()
            Else
                Return input.Substring(startIndex).Trim()
            End If
        End If
        Return String.Empty
    End Function

    Private Sub DisplayCurrentDate()

        Dim currentDate As DateTime = DateTime.Now

        Dim formattedDate As String = currentDate.ToString("dddd, MMMM dd, yyyy")

        labelDate.Text = formattedDate
    End Sub
    Private Sub btnExit_Click(sender As Object, e As EventArgs) Handles btnExit.Click
        Try
            If serialPort1.IsOpen Then
                serialPort1.Close()
            End If

            Me.Close()

            Environment.Exit(0)

        Catch ex As Exception
            MessageBox.Show("Error Closing Serial Port", "Error opening Serial Port", MessageBoxButtons.OK, MessageBoxIcon.Warning)
        End Try
    End Sub


End Class
