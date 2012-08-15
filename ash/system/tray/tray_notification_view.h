// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ASH_SYSTEM_TRAY_TRAY_NOTIFICATION_VIEW_H_
#define ASH_SYSTEM_TRAY_TRAY_NOTIFICATION_VIEWS_H_

#include "ui/views/controls/button/image_button.h"
#include "ui/views/view.h"

namespace gfx {
class ImageSkia;
}

namespace views {
class ImageView;
}

namespace ash {

class SystemTrayItem;

namespace internal {

// A view for closable notification views, laid out like:
//  -------------------
// | icon  contents  x |
//  ----------------v--
// The close button will call OnClose() when clicked.
class TrayNotificationView : public views::View,
                             public views::ButtonListener {
 public:
  // If icon_id is 0, no icon image will be set. SetIconImage can be called
  // to later set the icon image.
  TrayNotificationView(SystemTrayItem* tray, int icon_id);
  virtual ~TrayNotificationView();

  // InitView must be called once with the contents to be displayed.
  void InitView(views::View* contents);

  // Sets/updates the icon image.
  void SetIconImage(const gfx::ImageSkia& image);

  // Replaces the contents view.
  void UpdateView(views::View* new_contents);

  // Replaces the contents view and updates the icon image.
  void UpdateViewAndImage(views::View* new_contents,
                          const gfx::ImageSkia& image);

  // Overridden from ButtonListener.
  virtual void ButtonPressed(views::Button* sender,
                             const ui::Event& event) OVERRIDE;

  // Overridden from views::View.
  virtual bool OnMousePressed(const ui::MouseEvent& event) OVERRIDE;
  virtual ui::GestureStatus OnGestureEvent(
      const ui::GestureEvent& event) OVERRIDE;

 protected:
  // Called when the close button is pressed. Does nothing by default.
  virtual void OnClose();
  // Called when the notification is clicked on. Does nothing by default.
  virtual void OnClickAction();

  SystemTrayItem* tray() { return tray_; }

 private:
  void HandleClose();
  void HandleClickAction();

  SystemTrayItem* tray_;
  int icon_id_;
  views::ImageView* icon_;

  DISALLOW_COPY_AND_ASSIGN(TrayNotificationView);
};

}  // namespace internal
}  // namespace ash

#endif  // ASH_SYSTEM_TRAY_TRAY_NOTIFICATION_VIEWS_H_
