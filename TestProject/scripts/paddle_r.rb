# frozen_string_literal: true

# Player2 paddle
class PaddleR < Borek::Scriptable
  def on_update(delta)
    axis = 0
    axis += 1 if Borek::Input.key_pressed? :up
    axis -= 1 if Borek::Input.key_pressed? :down
    transform.position.y += axis * 600 * delta
  end
end
