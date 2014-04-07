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

NavigationPane {
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
            Label {
                id: lblMsg
                objectName: "lblMsg"
            }

            RadioGroup {
                id: rdgActions
                objectName: "rdgActions"
            }
            Container {
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
            }

        }
    }
}