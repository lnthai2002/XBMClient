/*
 * Copyright (c) 2011-2013 BlackBerry Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import bb.cascades 1.2

Page {
    Container {
        horizontalAlignment: HorizontalAlignment.Left
        verticalAlignment: VerticalAlignment.Top
        layout: StackLayout {
            orientation: LayoutOrientation.TopToBottom
        }
        topPadding: 20.0
        bottomPadding: 20.0
        leftPadding: 20.0
        rightPadding: 20.0
        TextField {
            id: txtHost
            objectName: "txtHost"
            hintText: "XBMC host address"
        }
        TextField {
            id: txtPort
            hintText: "XBMC listening port"
        }
        TextField {
            id: txtUser
            hintText: "XBMC username"
        }
        TextField {
            id: txtPass
            hintText: "XBMC password"
            inputMode: TextFieldInputMode.Password
        }
        Label {
            id: lblUrl
            objectName: "lblUrl"
        }

        Container {
        	layout: StackLayout {
        		orientation: LayoutOrientation.LeftToRight
        	}
            Button {
                id: btnSave
                objectName: "btnRegister"
                text: "Register"
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 5
                }
                verticalAlignment: VerticalAlignment.Top
                horizontalAlignment: HorizontalAlignment.Left
            }
            Button {
                id: btnSend
                objectName: "btnSend"
                text: "Send"
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 5.0
                }
                verticalAlignment: VerticalAlignment.Top
                horizontalAlignment: HorizontalAlignment.Right
                accessibility.name: "btnSend"
            
            }	
        }

    }
}
